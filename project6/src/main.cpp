#include<string> 
#include<iostream>
#include<fstream>

#include "Parser.h"
#include "Code.h"

using namespace std;

//takes two arguments: source filename, target filename
//translates the assembly language in the source file to machine language in the target file 
int main(int argc, char* argv[])
{
	if(argc!=3){
		cout<<"wrong number of arguments\n";
	} else{
		ofstream file_out{argv[2]};
		string filename(argv[1]);
		Code code;
		{
			Parser parser(filename);
			while(parser.is_good()){
				Command_type command_type=parser.get_type();
				if(command_type==Command_type::Label){
					code.add_label(parser.get_label(),parser.get_next_line_num());
				}	
				parser.advance();
			}
		}
		{
			Parser parser(filename);
			while(parser.is_good()){
				Command_type command_type=parser.get_type();
				string instruction;
				switch(command_type){
					case(Command_type::A):		instruction="0"+code.address(parser.get_address());
									file_out<<instruction<<endl;
									break;
					case(Command_type::C):		instruction="111"+code.comp(parser.get_comp())+code.dest(parser.get_dest())+code.jump(parser.get_jump());
									file_out<<instruction<<endl;
									break;	
					case(Command_type::Label):	break;		
				}
				parser.advance();		
			}
		}
	}
}     
