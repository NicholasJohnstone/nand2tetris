#include "Parser.h"

#include <sstream>

Parser::Parser(string filename)
	: file_stream{filename}
{
	if (file_stream)
	{
		advance();
	}
}

void Parser::advance()
{
	string line;
	bool found = false;
	while (!found && getline(file_stream, line))
	{
		istringstream line_stream{line};
		string command;
		line_stream >> command;
		map<string, Command_type>::const_iterator type_itr = string_to_command.find(command);
		if (type_itr != string_to_command.end())
		{
			found = true;
			type = (*type_itr).second;
			switch (type)
			{
			//commands in the form: type-string-number
			case (C_PUSH):
			case (C_POP):
			case (C_CALL):
			case (C_FUNCTION):
				if (!(line_stream >> arg1 >> arg2))
				{
					file_stream.setstate(ios::failbit);
				}
				break;
			//commands in the form: type-string
			case (C_LABEL):
			case (C_GOTO):
			case (C_IF):
				if (!(line_stream >> arg1))
				{
					file_stream.setstate(ios::failbit);
				}
				break;
			//commands in the form: type 	(eg Arithmetic and return commands)				
			default:
				break;
			}
		}
	}
}

bool Parser::is_good() const
{
	if (file_stream)
	{
		return true;
	}
	else
	{
		return false;
	}
}

Command_type Parser::get_type() const
{
	return type;
}

string Parser::get_arg1() const
{
	return arg1;
}

int16_t Parser::get_arg2() const
{
	return arg2;
}