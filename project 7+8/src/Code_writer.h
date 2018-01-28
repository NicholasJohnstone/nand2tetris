#ifndef CODE_WRITER_H_
#define CODE_WRITER_H_

#include <iostream>
#include <fstream>
#include <string>
#include <map>

#include "Command_type.h"

using namespace std;

class Code_writer
{
public:
  Code_writer(string filename);
  void set_fileName(string filename);
  //initialisation
  void write_init();
  //Stack arithmetic
  void write_arithmetic(Command_type command);
  //Memory access
  void write_push_pop(Command_type command, string segment, int16_t index);
  //Program flow
  void write_label(string label);
  void write_goto(string label);
  void write_if(string label);
  //Function calling
  void write_call(string called_function_name,int16_t num_args);
  void write_return();
  void write_function(string new_function_name,int16_t num_locals);


 
private:
  ofstream file_stream;
  string s_filename;
  string function_name;
  int16_t label_count = 0;

  string get_full_label(string label);

  void write_unitary(char op); 
  void write_binary(char op);
  void write_comparator(string op);

  void write_push_D();
  void write_pop_D();
  void write_value_to_D(string segment,int16_t index);
  void write_address_to_D(string segment,int16_t index);
  void write_pop_address(string address);
  void write_push_address(string address);
  void write_pop_address(string segment,int16_t index);
  void write_push_address(string segment,int16_t index);

  const map<string, string> string_to_segment = {
      {"argument", "ARG"},
      {"local", "LCL"},
      {"this", "THIS"},
      {"that", "THAT"}};
};

#endif //CODE_WRITER_H_
