#include "Code.h"

#include<bitset>
#include<iostream>
#include<sstream>

Code::Code(){
	dest_table.insert({
		{"","000"},{"M","001"},{"D","010"},{"MD","011"},{"A","100"},{"AM","101"},{"AD","110"},{"AMD","111"}
	});
	comp_table.insert({
		{"0","0101010"},{"1","0111111"},{"-1","0111010"},{"D","0001100"},{"!D","0001101"},{"-D","0001111"},{"D+1","0011111"},{"D-1","0001110"},
		{"A","0110000"},{"!A","0110001"},{"-A","0110011"},{"A+1","0110111"},{"A-1","0110010"},{"D+A","0000010"},{"D-A","0010011"},{"A-D","0000111"},{"D&A","0000000"},{"D|A","0010101"},
		{"M","1110000"},{"!M","1110001"},{"-M","1110011"},{"M+1","1110111"},{"M-1","1110010"},{"D+M","1000010"},{"D-M","1010011"},{"M-D","1000111"},{"D&M","1000000"},{"D|M","1010101"}
	});
	jump_table.insert({
		{"","000"},{"JGT","001"},{"JEQ","010"},{"JGE","011"},{"JLT","100"},{"JNE","101"},{"JLE","110"},{"JMP","111"}
	});
	symbol_table.insert({
		{"SCREEN","100000000000000"},{"KBD","110000000000000"},{"SP","000000000000000"},{"LCL","000000000000001"},{"ARG","000000000000010"},{"THIS","000000000000011"},{"THAT","000000000000100"}
	});	
	for(unsigned int i=0;i<16;i++){
		bitset<15> x{i};
		ostringstream temp;
		temp<<x;
		symbol_table.insert({"R"+to_string(i),temp.str()});
	}
}



string Code::address(const string& value){
	if(isdigit(value[0])){
		istringstream value_stream{value};
		unsigned int temp;
		value_stream>>temp;
		return int_to_binary(temp);
	} else if(symbol_table.find(value)!=symbol_table.end()){
		return symbol_table.at(value);
	} else{
		string new_code=int_to_binary(n);
		n++;
		symbol_table.insert({value,new_code});
		return new_code;
	}
}

string Code::comp(const string& comp) const{
	return comp_table.at(comp);			
}

string Code::dest(const string& dest) const{
	return dest_table.at(dest);			
}

string Code::jump(const string& jump) const{
	return jump_table.at(jump);			
}

void Code::add_label(const string& label,unsigned int pos){
	symbol_table.insert({label,int_to_binary(pos)});
}

string Code::int_to_binary(unsigned int i) const{
		bitset<15> bits{i};
		ostringstream bit_stream;
		bit_stream<<bits;
		return bit_stream.str();
}
