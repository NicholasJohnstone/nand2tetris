#ifndef COMPILATION_ENGINE_
#define COMPILATION_ENGINE_

#include <string>
#include <iostream>
#include <fstream>

#include "Jack_tokenizer.h"

using namespace std;

/* this class follows the convection the functions returning void are expected to throw an exception if unable to compile
// whereas functions that return a bool are expected to leave the filestream untouched and return false if unable to compile
*/
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
  void compile_expression_list();

private:
  Jack_tokenizer tokenizer;
  ofstream out_stream;

  bool compile_statement();

  bool read_structure_opening(string structure_name, set<Keyword> keyword_list);
  bool read_specific_keyword(Keyword required_keyword);
  bool read_type();
  bool read_identifier();
  bool read_specific_char(char required_char);
  bool read_char(set<char> char_set);

  void print_identifier();
  void print_keyword();
  void print_symbol();
  void print_int_val();
  void print_string_val();
};

#endif //COMPILATION_ENGINE_