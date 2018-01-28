#ifndef COMMAND_TYPE_H_
#define COMMAND_TYPE_H_

#include <string>
#include <map>

using namespace std;

enum Command_type
{
  C_ADD,
  C_SUB,
  C_NEG,
  C_EQ,
  C_GT,
  C_LT,
  C_AND,
  C_OR,
  C_NOT,
  C_PUSH,
  C_POP,
  C_LABEL,
  C_GOTO,
  C_IF,
  C_FUNCTION,
  C_RETURN,
  C_CALL
};

const map<string, Command_type> string_to_command = {
    {"add", C_ADD},
    {"sub", C_SUB},
    {"neg", C_NEG},
    {"eq", C_EQ},
    {"gt", C_GT},
    {"lt", C_LT},
    {"and", C_AND},
    {"or", C_OR},
    {"not", C_NOT},
    {"push", C_PUSH},
    {"pop", C_POP},
    {"label", C_LABEL},
    {"goto", C_GOTO},
    {"if-goto", C_IF},
    {"function", C_FUNCTION},
    {"return", C_RETURN},
    {"call", C_CALL}
    };

enum Segment_type
{
  C_ARGUMENT,
  C_LOCAL,
  C_STATIC,
  C_CONSTANT,
  C_THIS,
  C_THAT,
  C_POINTER,
  C_TEMP
};

const map<string, Segment_type> string_to_arithmetic = {
    {"argument", C_ARGUMENT},
    {"local", C_LOCAL},
    {"constant", C_CONSTANT},
    {"this", C_THIS},
    {"that", C_THAT},
    {"pointer", C_POINTER},
    {"temp", C_TEMP}};

#endif //COMMAND_TYPE_H_