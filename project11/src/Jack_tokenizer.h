#ifndef JACK_TOKENIZER_H_
#define JACK_TOKENIZER_H_

#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <set>

using namespace std;

enum Token_type
{
    T_KEYWORD,
    T_SYMBOL,
    T_IDENTIFIER,
    T_INT_CONST,
    T_STRING_CONST
};

enum Keyword
{
    K_CLASS,
    K_METHOD,
    K_FUNCTION,
    K_CONSTRUCTOR,
    K_INT,
    K_BOOLEAN,
    K_CHAR,
    K_VOID,
    K_VAR,
    K_STATIC,
    K_FIELD,
    K_LET,
    K_DO,
    K_IF,
    K_ELSE,
    K_WHILE,
    K_RETURN,
    K_TRUE,
    K_FALSE,
    K_NULL,
    K_THIS,
    K_NONE
};


class Jack_tokenizer
{
  public:
    Jack_tokenizer(string input_flename);

    bool is_good();
    void advance();

    Token_type get_token_type();
    Keyword get_keyword();
    char get_symbol();
    string get_idenifier();
    int get_int_val();
    string get_string_val();

  private:
    ifstream input_stream;

    Token_type token_type;
    Keyword keyword;
    char symbol = ' ';
    string idenfier;
    int int_val = 0;
    string string_val;

    string read_word();
    void read_string();
    bool ignore_possible_comment();
};

const map<string, Keyword> string_to_keyword = {
    {"class", K_CLASS},
    {"method", K_METHOD},
    {"function", K_FUNCTION},
    {"constructor", K_CONSTRUCTOR},
    {"int", K_INT},
    {"boolean", K_BOOLEAN},
    {"char", K_CHAR},
    {"void", K_VOID},
    {"var", K_VAR},
    {"static", K_STATIC},
    {"field", K_FIELD},
    {"let", K_LET},
    {"do", K_DO},
    {"if", K_IF},
    {"else", K_ELSE},
    {"while", K_WHILE},
    {"return", K_RETURN},
    {"true", K_TRUE},
    {"false", K_FALSE},
    {"null", K_NULL},
    {"this", K_THIS}};

const set<char> symbol_set = {'{', '}', '(', ')', '[', ']', '.', ',', ';', '+', '-', '*', '/', '&', '|', '<', '>', '=', '~'};

#endif //JACK_TOKENIZER_H_