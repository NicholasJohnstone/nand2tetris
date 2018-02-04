#ifndef COMPILATION_ENGINE_
#define COMPILATION_ENGINE_

#include <string>
#include <iostream>
#include <fstream>

#include "Jack_tokenizer.h"
#include "Symbol_table.h"
#include "VM_writer.h"

using namespace std;

class Compilation_engine
{
public:
  Compilation_engine(string input_filename, string output_filename);
  void run();

  void compile_class();
  bool compile_class_var_dec();
  bool compile_subroutine();
  void compile_parameter_list();
  bool compile_var_dec();
  void compile_statements();
  bool compile_do();
  bool compile_let();
  bool compile_while();
  bool compile_return();
  bool compile_if();
  void compile_expression();
  void compile_term();
  int compile_expression_list();

private:
  Jack_tokenizer tokenizer;
  Symbol_table symbol_table;
  VM_writer vm_writer;

  string class_name;
  int loop_count=0;
  bool returns_void;

  bool compile_statement();
  void compile_subroutine_call(string identifier_1);

  void write_push(string identifier);
  void write_pop(string identifier);

  Keyword read_keyword();
  bool read_specific_keyword(Keyword required_keyword);
  string read_type();
  string read_identifier();
  char read_char();
  bool read_specific_char(char required_char);
};

#endif //COMPILATION_ENGINE_