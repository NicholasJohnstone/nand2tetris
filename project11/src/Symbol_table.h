#ifndef SYMBOL_TABLE_H_
#define SYMBOL_TABLE_H_

#include <string>
#include <map>

using namespace std;

enum Kind
{
    KI_STATIC,
    KI_FIELD,
    KI_ARG,
    KI_VAR,
    KI_NONE
};

class Symbol_table
{
  public:
    void start_subroutine();
    void define(string name, string type, Kind kind);
    int var_count(Kind kind);
    Kind kind_of(string name);
    string type_of(string name);
    int index_of(string name);

  private:
    map<string,tuple<string,Kind,int>> class_table;
    map<string,tuple<string,Kind,int>> method_table;

    int symbol_count[4]={};     //initialize to array of 0's
};

#endif //SYMBOL_TABLE_H