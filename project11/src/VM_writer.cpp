#include "VM_writer.h"

VM_writer::VM_writer(string out_filename)
    : out_stream{out_filename} {}

void VM_writer::write_push(Segment segment, int16_t index)
{
    out_stream << "push ";
    switch (segment)
    {
    case (S_ARG):
        out_stream << "argument ";
        break;
    case (S_CONST):
        out_stream << "constant ";
        break;
    case (S_LOCAL):
        out_stream << "local ";
        break;
    case (S_POINTER):
        out_stream << "pointer ";
        break;
    case (S_STATIC):
        out_stream << "static ";
        break;
    case (S_TEMP):
        out_stream << "temp ";
        break;
    case (S_THAT):
        out_stream << "that ";
        break;
    case (S_THIS):
        out_stream << "this ";
        break;
    }
    out_stream << index << endl;
}

void VM_writer::write_pop(Segment segment, int16_t index)
{
    out_stream << "pop ";
    switch (segment)
    {
    case (S_ARG):
        out_stream << "argument ";
        break;
    case (S_CONST):
        out_stream << "constant ";
        break;
    case (S_LOCAL):
        out_stream << "local ";
        break;
    case (S_POINTER):
        out_stream << "pointer ";
        break;
    case (S_STATIC):
        out_stream << "static ";
        break;
    case (S_TEMP):
        out_stream << "temp ";
        break;
    case (S_THAT):
        out_stream << "that ";
        break;
    case (S_THIS):
        out_stream << "this ";
        break;
    }
    out_stream << index << endl;
}

void VM_writer::write_arithmetic(Command command)
{
    switch (command)
    {
    case (C_ADD):
        out_stream << "add\n";
        break;
    case (C_SUB):
        out_stream << "sub\n";
        break;
    case (C_NEG):
        out_stream << "neg\n";
        break;
    case (C_EQ):
        out_stream << "eq\n";
        break;
    case (C_GT):
        out_stream << "gt\n";
        break;
    case (C_LT):
        out_stream << "lt\n";
        break;
    case (C_AND):
        out_stream << "and\n";
        break;
    case (C_OR):
        out_stream << "or\n";
        break;
    case (C_NOT):
        out_stream << "not\n";
        break;
    }
}

void VM_writer::write_label(string label)
{
    out_stream << "label " << label << endl;
}

void VM_writer::write_goto(string label)
{
    out_stream << "goto " << label << endl;
}

void VM_writer::write_if(string label)
{
    out_stream << "if-goto " << label << endl;
}

void VM_writer::write_call(string name, int16_t num_args)
{
    out_stream << "call " << name << ' ' << num_args << endl;
}

void VM_writer::write_function(string name, int16_t num_locals)
{
    out_stream << "function " << name << ' ' << num_locals << endl;
}

void VM_writer::write_return()
{
    out_stream << "return\n";
}