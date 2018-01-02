#include<string> 
#include<iostream>

#include "Parser.h"

using namespace std;

//takes 1 argument, source filename
//outputs the fields for each pseudo-command 
int main(int argc, char* argv[])
{
	if(argc==1){
		cout<<"must pass filename in command\n";
	} else{
		string filename(argv[1]);
		Parser parser(filename);
		while(parser.is_good()){
			cout<<"Instruction number: "<<parser.get_next_line_num()-1<<endl;
			Command_type command_type=parser.get_type();
			switch(command_type){
				case(Command_type::A):		cout<<"Type: A\n";
								cout<<"Address: "<<parser.get_address()<<endl;
								break;
				case(Command_type::C):		cout<<"Type: C\n";
								cout<<"Dest: "<<parser.get_dest()<<endl;
								cout<<"Comp: "<<parser.get_comp()<<endl;
								cout<<"Jump: "<<parser.get_jump()<<endl;
								break;				
				case(Command_type::Label):	cout<<"Type: Label\n";
								cout<<"Label: "<<parser.get_label()<<endl;
								break;
			}
			cout<<endl;
			parser.advance();		
		}
	}
}     
