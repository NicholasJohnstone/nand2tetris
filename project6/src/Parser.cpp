#include "Parser.h"

#include<sstream>

Parser::Parser(string filename)
	:file_stream{filename} 
	{
	if(file_stream){
		advance();
	}
}

void Parser::advance(){
	string line;
	while(getline(file_stream,line)){
		istringstream line_stream{line};
		line_stream>>ws;
		getline(line_stream,line,'/');
		if(line.length()!=0){
			char token =line[0];
			if(token!='\r'){
				switch(token){
				case('@') : 	a_statement(line);
						break;
				case('(') :	label(line);
					 	break;
				case('\r'):	break; 
				default :	c_statement(line);
						break;
				}
				break;
			}
		}
	}	  	 	
}

void Parser::a_statement(string line){
	type=Command_type::A;	
	istringstream line_stream{line};
	line_stream.get();
	line_stream>>field[0];			
	next_line_num++;
}

void Parser::c_statement(string line){
	type=Command_type::C;
	istringstream line_stream{line};
	getline(line_stream,line,';');
	if(!(line_stream>>field[2])){
		field[2]="";
	}
	istringstream clause_stream{line};
	getline(clause_stream,line,'=');
	if(clause_stream>>field[1]){
		istringstream dest_stream{line};
		dest_stream>>field[0];
	} else {
		istringstream comp_stream{line};;
		comp_stream>>field[1];
		field[0]="";
	}			
	next_line_num++;
}

void Parser::label(string line){
	type=Command_type::Label;
	istringstream line_stream{line};
	line_stream.get();
	getline(line_stream,line,')');
	istringstream label_stream{line};
	label_stream>>field[0];
}

unsigned int Parser::get_next_line_num() const{
	return next_line_num;
}

Command_type Parser::get_type() const{
	return type;
}

string Parser::get_address() const{
	return field[0];
}

string Parser::get_dest() const{
	return field[0];
}

string Parser::get_comp() const{
	return field[1];
}

string Parser::get_jump() const{
	return field[2];
}

string Parser::get_label() const{
	return field[0];
}

bool Parser::is_good() const{
	if(file_stream){
		return true;
	} else {
		return false;
	}
}
