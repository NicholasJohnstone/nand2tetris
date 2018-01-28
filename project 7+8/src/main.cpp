#include <string>
#include <iostream>

#include "Parser.h"
#include "Code_writer.h"

using namespace std;

int main(int argc, char *argv[])
{
    if(argc==1){
        return 0;
    }
    string out_filename(argv[1]);
    Code_writer writer(out_filename);
    writer.write_init();
    for (int i = 2; i < argc; i++)
    {
        string in_filename(argv[i]);
        Parser parser(in_filename);
        writer.set_fileName(in_filename);
        while (parser.is_good())
        {
            Command_type type = parser.get_type();
            switch (type)
            {
            case (C_PUSH):
            case (C_POP):
                writer.write_push_pop(type, parser.get_arg1(), parser.get_arg2());
                break;
            case (C_LABEL):
                writer.write_label(parser.get_arg1());
                break;
            case (C_GOTO):
                writer.write_goto(parser.get_arg1());
                break;
            case (C_IF):
                writer.write_if(parser.get_arg1());
                break;
            case (C_FUNCTION):
                writer.write_function(parser.get_arg1(), parser.get_arg2());
                break;
            case (C_CALL):
                writer.write_call(parser.get_arg1(), parser.get_arg2());
                break;
            case (C_RETURN):
                writer.write_return();
                break;
            default:
                writer.write_arithmetic(type);
            }
            parser.advance();
        }
    }
}