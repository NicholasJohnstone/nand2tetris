#ifndef PARSER_H_
#define PARSER_H_

#include <iostream>
#include <fstream>
#include <string>
#include <map>

#include "Command_type.h"

using namespace std;

class Parser
{
public:
  Parser(string filename);

  //reads in next command if possible
  void advance();

  //true iff last use of advance() read in a command
  bool is_good() const;

  //for all commands
  Command_type get_type() const;

  //for commmands with at least 1 arg
  string get_arg1() const;

  //for commands with 2 args
  int16_t get_arg2() const;

private:
  ifstream file_stream;
  Command_type type;
  string arg1;
  int16_t arg2;
};

#endif //PARSER_H_
