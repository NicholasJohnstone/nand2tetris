#include "Code_writer.h"

#include <sstream>

Code_writer::Code_writer(string filename)
    : file_stream{filename}
{
}

void Code_writer::set_fileName(string filename)
{
    int start_index = filename.find_last_of('/') + 1;
    s_filename = filename.substr(start_index, filename.find_last_of('.') - start_index);
}

void Code_writer::write_init()
{
    file_stream << "@256\nD=A\n@SP\nM=D\n"; //set SP = 256
    write_call("Sys.init", 0);
}

void Code_writer::write_arithmetic(Command_type command)
{
    switch (command)
    {
        //unitary
    case (C_NEG):
        write_unitary('-');
        break;
    case (C_NOT):
        write_unitary('!');
        break;
    //binary
    case (C_ADD):
        write_binary('+');
        break;
    case (C_SUB):
        write_binary('-');
        break;
    case (C_AND):
        write_binary('&');
        break;
    case (C_OR):
        write_binary('|');
        break;
    //comp
    case (C_EQ):
        write_comparator("EQ");
        break;
    case (C_GT):
        write_comparator("GT");
        break;
    case (C_LT):
        write_comparator("LT");
        break;
    default:
        throw invalid_argument("not a valid arthimetic command");
        break;
    }
}

void Code_writer::write_push_pop(Command_type command, string segment, int16_t index)
{
    if (command != C_PUSH && command != C_POP)
    {
        throw invalid_argument("not a valid push/pop command");
    }
    map<string, string>::const_iterator segment_itr = string_to_segment.find(segment);
    if (segment_itr != string_to_segment.end())
    {
        string seg = (*segment_itr).second;
        if (command == C_PUSH)
        {
            write_push_address(seg, index);
        }
        else
        {
            write_pop_address(seg, index);
        }
    }
    else if (segment == "constant")
    {
        if (command == C_PUSH)
        {
            file_stream << '@' << index << "\nD=A\n"; //write index to D
            write_push_D();                           //push D
        }
        else
        {
            throw invalid_argument("cannot pop constant");
        }
    }
    else
    {
        string address;
        if (segment == "static")
        {
            address = s_filename + '.' + to_string(index);
        }
        else if (segment == "pointer")
        {
            address = to_string(3 + index);
        }
        else if (segment == "temp")
        {
            address = to_string(5 + index);
        }
        else
        {
            throw invalid_argument("push_pop segment not valid");
        }
        if (command == C_PUSH)
        {
            write_push_address(address);
        }
        else
        {
            write_pop_address(address);
        }
    }
}

void Code_writer::write_label(string label)
{
    file_stream << '(' << get_full_label(label) << ")\n";
}
void Code_writer::write_goto(string label)
{
    file_stream << '@' << get_full_label(label) << "\n";
    file_stream << "D;JMP\n";
}

void Code_writer::write_if(string label)
{
    write_pop_D();
    file_stream << '@' << get_full_label(label) << "\n";
    file_stream << "D;JNE\n";
}

void Code_writer::write_call(string called_function_name, int16_t num_args)
{
    file_stream << "@RETURN." << label_count << "\nD=A\n"; //set D to ret address
    write_push_D();

    write_push_address("LCL");  //push LCL
    write_push_address("ARG");  //push ARG
    write_push_address("THIS"); //push THIS
    write_push_address("THAT"); //push THAT

    file_stream << "@SP\nD=M\n";                                    //set D to SP
    file_stream << '@' << (num_args + 5) << "\nD=D-A\n@ARG\nM=D\n"; //set ARG to SP-n-5

    file_stream << "@SP\nD=M\n@LCL\nM=D\n";                    //set LCL to SP
    file_stream << '@' << called_function_name << "\nD;JMP\n"; //jump to function
    file_stream << '(' << "RETURN." << label_count << ")\n";   //label return
    label_count++;
}

//uses R14 and R15
void Code_writer::write_return()
{
    write_value_to_D("LCL", -5);            //set D to return address
    file_stream << "@R14\nM=D\n";           //save return address to R14
    write_pop_address("ARG", 0);            //pop return value
    write_address_to_D("ARG", 1);           //set D to ARG+1
    file_stream << "@R15\nM=D\n";           //save ARG+1 to R15
    file_stream << "@LCL\nD=M\n@SP\nM=D\n"; //set SP to LCL
    write_pop_address("THAT");
    write_pop_address("THIS");
    write_pop_address("ARG");
    write_pop_address("LCL");
    file_stream << "@R15\nD=M\n@SP\nM=D\n"; //set SP to old ARG+1
    file_stream << "@R14\nA=M\nD;JMP\n";    //jump to return address
}

void Code_writer::write_function(string new_function_name, int16_t num_locals)
{
    function_name = new_function_name;
    file_stream << '(' << function_name << ")\n";                 //label function
    file_stream << '@' << num_locals << "\nD=A\n";                //set D to num
    file_stream << '@' << get_full_label("0done") << "\nD;JEQ\n"; //jump to end if D=0
    write_label("0loop");                                         //label start of loop
    file_stream << "@SP\nM=M+1\nA=M-1\nM=0\nD=D-1\n";             //loop(push 0,decrement D)
    file_stream << '@' << get_full_label("0loop") << "\nD;JNE\n"; //restart loop if D!=0
    write_label("0done");                                         //label end
}

string Code_writer::get_full_label(string label)
{
    return function_name + "$" + label;
}

void Code_writer::write_unitary(char op)
{
    file_stream << "@SP\nA=M-1\nM=" << op << "M\n";
}
void Code_writer::write_binary(char op)
{
    file_stream << "@SP\nAM=M-1\nD=M\nA=A-1\nM=M" << op << "D\n";
}
void Code_writer::write_comparator(string op)
{
    file_stream << "@SP\nAM=M-1\nD=M\nA=A-1\nD=M-D\nM=-1\n@MATH." << label_count << "\nD ; J" << op << "\n@SP\nA=M-1\nM=0\n(MATH." << label_count << ")\n";
    label_count++;
}

void Code_writer::write_push_D()
{
    file_stream << "@SP\nM=M+1\nA=M-1\nM=D\n";
}

void Code_writer::write_pop_D()
{
    file_stream << "@SP\nAM=M-1\nD=M\n";
}

void Code_writer::write_value_to_D(string segment, int16_t index)
{
    if (index >= 0)
    {
        file_stream << '@' << segment << "\nD=M\n";      //set D to segment address
        file_stream << '@' << index << "\nA=D+A\nD=M\n"; //set D to *(segment+index)
    }
    else
    {
        file_stream << '@' << segment << "\nD=M\n";       //set D to segment address
        file_stream << '@' << -index << "\nA=D-A\nD=M\n"; //set D to *(segment+index)
    }
}

void Code_writer::write_address_to_D(string segment, int16_t index)
{
    if (index >= 0)
    {
        file_stream << '@' << segment << "\nD=M\n"; //set D to segment address
        file_stream << '@' << index << "\nD=D+A\n"; //set D to segment+index address
    }
    else
    {
        file_stream << '@' << segment << "\nD=M\n";  //set D to segment address
        file_stream << '@' << -index << "\nD=D-A\n"; //set D to segment+index address
    }
}

void Code_writer::write_pop_address(string address)
{
    write_pop_D();                              //pop to D
    file_stream << '@' << address << "\nM=D\n"; //copy D to address
}
void Code_writer::write_push_address(string address)
{
    file_stream << '@' << address << "\nD=M\n"; //copy value to D
    write_push_D();                             //push D
}

//uses R13
void Code_writer::write_pop_address(string segment, int16_t index)
{
    write_address_to_D(segment, index);
    file_stream << "@R13\nM=D\n"; //use R13 to store address
    write_pop_D();
    file_stream << "@R13\nA=M\nM=D\n"; //write D to address in R13
}
void Code_writer::write_push_address(string segment, int16_t index)
{
    write_value_to_D(segment, index);
    write_push_D();
}