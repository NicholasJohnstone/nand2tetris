#ifndef CODE_H_
#define CODE_H_

#include<string>
#include<map>
#include<unordered_map>

using namespace std;

//translates command fields into their binary representation
//keeps symbol table for variables and labels 
class Code{
	public: 
		Code();
		
		//for A commands
		string address(const string& value);

		//for C commands
		string comp(const string& comp) const;
		string dest(const string& dest) const ;
		string jump(const string& jump) const;

		void add_label(const string& label,unsigned int pos);
	private:
		unsigned int n=16;			//location of next variable to be declared
		unordered_map<string,string> symbol_table;
		map<string,string> dest_table;
		map<string,string> comp_table;
		map<string,string> jump_table;

		string int_to_binary(unsigned int i) const;
};

#endif //CODE_H_
