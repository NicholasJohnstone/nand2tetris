#ifndef PARSER_H_
#define PARSER_H_

#include<iostream>
#include<fstream>
#include<string>

using namespace std;

enum class Command_type {A,C,Label};

//reads (pseudo-)commands line-by line
//breaks each command into it's fields
//value of string returned when calling wrong field for current command is undefined
class Parser{
	public:
		Parser(string filename);
		
		//reads in next command if possible
		void advance();

		//true iff last use of advance() read in a command  
		bool is_good() const;
	
		//for all commands
		unsigned int get_next_line_num() const;
		Command_type get_type() const;

		//for A commands
		string get_address() const ;

		//for C commands
		string get_dest() const;
		string get_comp() const;
		string get_jump() const ;
	
		//for Label commands
		string get_label() const;

	private:
		ifstream file_stream;
		string field[3];
		Command_type type;
		unsigned int next_line_num=0;
		bool state=false;

		void a_statement(string line);
		void c_statement(string line);
		void label(string line);
};


#endif //PARSER_H_
