

#include "Jack_tokenizer.h"

Jack_tokenizer::Jack_tokenizer(string input_flename)
    : input_stream{input_flename}
{
    if (input_stream)
    {
        advance();
    }
}
void Jack_tokenizer::advance()
{
    char next = input_stream.get();
    while (isspace(next) || next == '/') //remove whitespace including comments
    {
        if (next == '/')
        {
            if (!ignore_possible_comment())
            {
                break;
            }
        }
        next = input_stream.get();
    }
    if (isdigit(next)) //int constant case
    {
        token_type = T_INT_CONST;
        input_stream.unget();
        input_stream >> int_val;
        return;
    }
    if (next == '"') //string constant case
    {
        token_type = T_STRING_CONST;
        read_string();
        return;
    }
    if (next == '_' || isalpha(next))
    {
        input_stream.unget();
        string word = read_word();
        map<string, Keyword>::const_iterator keyword_itr = string_to_keyword.find(word);
        if (keyword_itr == string_to_keyword.end()) //idenifier case
        {
            token_type = T_IDENTIFIER;
            idenfier = word;
            return;
        }
        else //keyword case
        {
            token_type = T_KEYWORD;
            keyword = (*keyword_itr).second;
            return;
        }
    }
    if (symbol_set.find(next) != symbol_set.end()) //symbol case
    {
        token_type = T_SYMBOL;
        symbol = next;
        return;
    }
    input_stream.setstate(ios::failbit);
}

string Jack_tokenizer::read_word()
{
    string word = "";
    char next = input_stream.get();
    while (isalnum(next) || next == '_')
    {
        word += next;
        next = input_stream.get();
    }
    input_stream.unget();
    return word;
}

void Jack_tokenizer::read_string()
{
    string_val = "";
    char next = input_stream.get();
    while (next != '"')
    {
        if (next == EOF)
        {
            throw invalid_argument("string never terminated");
        }
        string_val += next;
        next = input_stream.get();
    }
}

bool Jack_tokenizer::ignore_possible_comment()
{
    char next = input_stream.get();
    if (next == '/') //normal comment
    {
        string temp;
        getline(input_stream, temp);
    }
    else if (next == '*') //multi line comment
    {
        while (true)
        {
            string temp;
            getline(input_stream, temp, '*');
            next = input_stream.get();
            if (next == '/')
            {
                break;
            }
        }
    }
    else
    {
        input_stream.unget(); //put unknown char back
        return false;
    }
    return true;
}

bool Jack_tokenizer::is_good()
{
    if (input_stream)
    {
        return true;
    }
    else
    {
        return false;
    }
}

Token_type Jack_tokenizer::get_token_type()
{
    return token_type;
}
Keyword Jack_tokenizer::get_keyword()
{
    return keyword;
}
char Jack_tokenizer::get_symbol()
{
    return symbol;
}
string Jack_tokenizer::get_idenifier()
{
    return idenfier;
}
int Jack_tokenizer::get_int_val()
{
    return int_val;
}
string Jack_tokenizer::get_string_val()
{
    return string_val;
}