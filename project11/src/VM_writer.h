#ifndef VM_WRITER_H_
#define VM_WRITER_H_

#include <string>
#include <fstream>

using namespace std;

enum Segment
{
    S_CONST,
    S_ARG,
    S_LOCAL,
    S_STATIC,
    S_THIS,
    S_THAT,
    S_POINTER,
    S_TEMP
};

enum Command
{
    C_ADD,
    C_SUB,
    C_NEG,
    C_EQ,
    C_GT,
    C_LT,
    C_AND,
    C_OR,
    C_NOT
};

class VM_writer
{
  public:
    VM_writer(string out_filename);
    void write_push(Segment segment, int16_t index);
    void write_pop(Segment segment, int16_t index);
    void write_arithmetic(Command command);
    void write_label(string label);
    void write_goto(string label);
    void write_if(string label);
    void write_call(string name, int16_t num_args);
    void write_function(string name, int16_t num_locals);
    void write_return();

  private:
    ofstream out_stream;
};

#endif //VM_WRITER_H_