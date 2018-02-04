#include "Compilation_engine.h"

Compilation_engine::Compilation_engine(string input_filename, string output_filename)
    : tokenizer{input_filename}, vm_writer{output_filename} {}

void Compilation_engine::run()
{
    while (tokenizer.is_good())
        compile_class();
}

void Compilation_engine::compile_class()
{
    if (!read_specific_keyword(K_CLASS))
        throw invalid_argument("found token outside class");
    class_name = read_identifier(); //set class name
    if (class_name == "")
        throw invalid_argument("class missing identifier");
    if (!read_specific_char('{'))
        throw invalid_argument("class missing opening '{'");
    while (compile_class_var_dec()) //read in class variables
        ;
    while (compile_subroutine()) //compile subroutines
        ;
    if (!read_specific_char('}'))
        throw invalid_argument("class missing closing '}'");
}

bool Compilation_engine::compile_class_var_dec()
{
    Keyword keyword = read_keyword();
    Kind kind;
    switch (keyword)
    {
    case (K_STATIC):
        tokenizer.advance();
        kind = KI_STATIC;
        break;
    case (K_FIELD):
        tokenizer.advance();
        kind = KI_FIELD;
        break;
    default:
        return false;
    }
    string type = read_type();
    if (type == "")
        throw invalid_argument("class var declaration missing type");
    string var_name = read_identifier();
    if (var_name == "")
        throw invalid_argument("class var declaration missing identifier");
    symbol_table.define(var_name, type, kind);
    while (read_specific_char(','))
    {
        string var_name = read_identifier();
        if (var_name == "")
            throw invalid_argument("class var declaration has extra ','");
        symbol_table.define(var_name, type, kind);
    }
    if (!read_specific_char(';'))
        throw invalid_argument("declared class var must be terminated by ';'");
    return true;
}

bool Compilation_engine::compile_subroutine()
{
    //check appropriate keyword
    Keyword keyword = read_keyword();
    switch (keyword)
    {
    case (K_CONSTRUCTOR):
    case (K_FUNCTION):
    case (K_METHOD):
        tokenizer.advance();
        break;
    default:
        return false;
    }
    //read return type and subroutine name
    loop_count = 0;
    returns_void = false;
    if (read_specific_keyword(K_VOID))
        returns_void = true;
    else if (read_type() == "")
        throw invalid_argument("subroutine must have return var type");
    string subroutine_name = read_identifier();
    if (subroutine_name == "")
        throw invalid_argument("subroutine missing identifier");

    if (!read_specific_char('('))
        throw invalid_argument("subroutine missing opening '(");

    //populating symbol table with args
    symbol_table.start_subroutine();
    if (keyword == K_METHOD) //if method then 1st arg is this
        symbol_table.define("this", class_name, KI_ARG);
    compile_parameter_list();

    if (!read_specific_char(')'))
        throw invalid_argument("subroutine missing closing ')");
    if (!read_specific_char('{'))
        throw invalid_argument("subroutine missing opening '{");

    //populate symbol table with vars
    while (compile_var_dec())
        ;

    //write function
    vm_writer.write_function(class_name + "." + subroutine_name, symbol_table.var_count(KI_VAR));
    if (keyword == K_METHOD) //if method then set this=arg[0]
    {
        vm_writer.write_push(S_ARG, 0);
        vm_writer.write_pop(S_POINTER, 0);
    }
    else if (keyword == K_CONSTRUCTOR) //if constructor then this=alloc[num_fields]
    {
        vm_writer.write_push(S_CONST, symbol_table.var_count(KI_FIELD));
        vm_writer.write_call("Memory.alloc", 1);
        vm_writer.write_pop(S_POINTER, 0);
    }
    compile_statements();

    if (!read_specific_char('}'))
        throw invalid_argument("subroutine missing closing '}");
    return true;
}

void Compilation_engine::compile_parameter_list()
{
    string type = read_type();
    if (type != "")
    {
        string arg_name = read_identifier();
        if (arg_name == "")
            throw invalid_argument("parameter missing identifier");
        symbol_table.define(arg_name, type, KI_ARG);
        while (read_specific_char(','))
        {
            type = read_type();
            if (type == "")
                throw invalid_argument("parameter list has extra ','");
            arg_name = read_identifier();
            if (arg_name == "")
                throw invalid_argument("parameter has missing identifier");
            symbol_table.define(arg_name, type, KI_ARG);
        }
    }
}

bool Compilation_engine::compile_var_dec()
{
    Keyword keyword = read_keyword();
    Kind kind;
    switch (keyword)
    {
    case (K_VAR):
        tokenizer.advance();
        kind = KI_VAR;
        break;
    default:
        return false;
    }
    string type = read_type();
    if (type == "")
        throw invalid_argument("var declaration missing type");
    string var_name = read_identifier();
    if (var_name == "")
        throw invalid_argument("var declaration missing identifier");
    symbol_table.define(var_name, type, kind);
    while (read_specific_char(','))
    {
        string var_name = read_identifier();
        if (var_name == "")
            throw invalid_argument("var declaration has extra ','");
        symbol_table.define(var_name, type, kind);
    }
    if (!read_specific_char(';'))
        throw invalid_argument("declared var must be terminated by ';'");
    return true;
}

void Compilation_engine::compile_statements()
{
    while (compile_statement())
        ;
}

bool Compilation_engine::compile_statement()
{
    return (compile_let() || compile_if() || compile_while() || compile_do() || compile_return());
}

bool Compilation_engine::compile_do()
{
    if (!read_specific_keyword(K_DO))
        return false;
    string identifier_1 = read_identifier();
    if (identifier_1 == "")
        throw invalid_argument("do statement missing function name");
    compile_subroutine_call(identifier_1);
    vm_writer.write_pop(S_TEMP, 0);
    if (!read_specific_char(';'))
        throw invalid_argument("subrouine call function missing terminating ';'");
    return true;
}

bool Compilation_engine::compile_let()
{
    if (!read_specific_keyword(K_LET))
        return false;
    string var_name = read_identifier();
    if (var_name == "")
        throw invalid_argument("let statement missing identifier");
    if (read_specific_char('['))
    {
        write_push(var_name);
        compile_expression();
        if (!read_specific_char(']'))
            throw invalid_argument("let statement missing closing ']'");
        vm_writer.write_arithmetic(C_ADD);
        vm_writer.write_pop(S_TEMP,0);
        if (!read_specific_char('='))
            throw invalid_argument("let statement missing '='");
        compile_expression();
        if (!read_specific_char(';'))
        {
            throw invalid_argument("let statement missing terminating ';'");
        }
        vm_writer.write_push(S_TEMP,0);
        vm_writer.write_pop(S_POINTER,1);
        vm_writer.write_pop(S_THAT, 0);
        return true;
    }
    if (!read_specific_char('='))
        throw invalid_argument("let statement missing '='");
    compile_expression();
    if (!read_specific_char(';'))
    {
        throw invalid_argument("let statement missing terminating ';'");
    }
    write_pop(var_name);
    return true;
}

bool Compilation_engine::compile_while()
{
    if (!read_specific_keyword(K_WHILE))
        return false;
    int count = loop_count;
    loop_count++;
    if (!read_specific_char('('))
        throw invalid_argument("while statement missing opening '('");
    vm_writer.write_label("while_start_" + to_string(count));
    compile_expression();
    vm_writer.write_arithmetic(C_NOT);
    vm_writer.write_if("while_end_" + to_string(count));
    if (!read_specific_char(')'))
        throw invalid_argument("while statement missing closing ')'");
    if (!read_specific_char('{'))
        throw invalid_argument("while statement missing opening '{'");
    compile_statements();
    vm_writer.write_goto("while_start_" + to_string(count));
    vm_writer.write_label("while_end_" + to_string(count));
    if (!read_specific_char('}'))
        throw invalid_argument("while statement missing closing '}'");
    return true;
}

bool Compilation_engine::compile_return()
{
    if (!read_specific_keyword(K_RETURN))
        return false;
    if (!read_specific_char(';'))
    {
        if (returns_void == true)
            throw invalid_argument("cannot return an expression when return type is void");
        compile_expression();
        if (!read_specific_char(';'))
            throw invalid_argument("return statement missing terminating ;");
    }
    else
    {
        if (returns_void == false)
            throw invalid_argument("must return appropriate type");
        vm_writer.write_push(S_CONST, 0);
    }
    vm_writer.write_return();
    return true;
}

bool Compilation_engine::compile_if()
{
    if (!read_specific_keyword(K_IF))
        return false;
    int count = loop_count;
    loop_count++;
    if (!read_specific_char('('))
        throw invalid_argument("if statement missing opening '('");
    compile_expression();
    vm_writer.write_arithmetic(C_NOT);
    vm_writer.write_if("if_end_" + to_string(count));
    if (!read_specific_char(')'))
        throw invalid_argument("if statement missing closing ')'");
    if (!read_specific_char('{'))
        throw invalid_argument("if statement missing opening '{'");
    compile_statements();
    if (!read_specific_char('}'))
        throw invalid_argument("if statement missing closing '}'");
    if (read_specific_keyword(K_ELSE))
    {
        if (!read_specific_char('{'))
            throw invalid_argument("else statement missing opening '{'");
        vm_writer.write_goto("else_end_" + to_string(count));
        vm_writer.write_label("if_end_" + to_string(count));
        compile_statements();
        vm_writer.write_label("else_end_" + to_string(count));
        if (!read_specific_char('}'))
            throw invalid_argument("else statement missing closing '}'");
    }
    else
        vm_writer.write_label("if_end_" + to_string(count));
    return true;
}

void Compilation_engine::compile_expression()
{
    compile_term();
    for (char next = read_char();; next = read_char())
    {
        switch (next)
        {
        case ('+'):
            tokenizer.advance();
            compile_term();
            vm_writer.write_arithmetic(C_ADD);
            break;
        case ('-'):
            tokenizer.advance();
            compile_term();
            vm_writer.write_arithmetic(C_SUB);
            break;
        case ('*'):
            tokenizer.advance();
            compile_term();
            vm_writer.write_call("Math.multiply", 2);
            break;
        case ('/'):
            tokenizer.advance();
            compile_term();
            vm_writer.write_call("Math.divide", 2);
            break;
        case ('&'):
            tokenizer.advance();
            compile_term();
            vm_writer.write_arithmetic(C_AND);
            break;
        case ('|'):
            tokenizer.advance();
            compile_term();
            vm_writer.write_arithmetic(C_OR);
            break;
        case ('<'):
            tokenizer.advance();
            compile_term();
            vm_writer.write_arithmetic(C_LT);
            break;
        case ('>'):
            tokenizer.advance();
            compile_term();
            vm_writer.write_arithmetic(C_GT);
            break;
        case ('='):
            tokenizer.advance();
            compile_term();
            vm_writer.write_arithmetic(C_EQ);
            break;
        default:
            return;
        }
    }
}

void Compilation_engine::compile_term()
{
    switch (tokenizer.get_token_type())
    {
    case (T_INT_CONST):
        vm_writer.write_push(S_CONST, tokenizer.get_int_val());
        tokenizer.advance();
        break;
    case (T_STRING_CONST):
    {
        string string_const = tokenizer.get_string_val();
        vm_writer.write_push(S_CONST, string_const.size());
        vm_writer.write_call("String.new", 1);
        for (unsigned int i = 0; i < string_const.size(); i++)
        {
            vm_writer.write_push(S_CONST, string_const[i]);
            vm_writer.write_call("String.appendChar", 2);
        }
        tokenizer.advance();
        break;
    }
    case (T_KEYWORD):
    {
        Keyword keyword = tokenizer.get_keyword();
        switch (keyword)
        {
        case (K_NULL):
        case (K_FALSE):
            vm_writer.write_push(S_CONST, 0);
            break;
        case (K_TRUE):
            vm_writer.write_push(S_CONST, 1);
            vm_writer.write_arithmetic(C_NEG);
            break;
        case (K_THIS):
            vm_writer.write_push(S_POINTER, 0);
            break;
        default:
            throw invalid_argument("disallowed keyword in term");
        }
        tokenizer.advance();
        break;
    }
    case (T_SYMBOL):
    {
        char symbol = tokenizer.get_symbol();
        switch (symbol)
        {
        case ('-'):
            tokenizer.advance();
            compile_term();
            vm_writer.write_arithmetic(C_NEG);
            break;
        case ('~'):
            tokenizer.advance();
            compile_term();
            vm_writer.write_arithmetic(C_NOT);
            break;
        case ('('):
            tokenizer.advance();
            compile_expression();
            if (!read_specific_char(')'))
                throw invalid_argument("term missing closing ')'");
            break;
        default:
            break; //note term may be followed by a variety of symbols
        }
        break;
    }
    case (T_IDENTIFIER):
        string identifier_1 = read_identifier();
        if (tokenizer.get_token_type() == T_SYMBOL && (tokenizer.get_symbol() == '.' || tokenizer.get_symbol() == '('))
            compile_subroutine_call(identifier_1);
        else
        { //var case
            write_push(identifier_1);
            if (read_specific_char('['))
            {
                compile_expression();
                if (!read_specific_char(']'))
                    throw invalid_argument("let statement missing closing ']'");
                vm_writer.write_arithmetic(C_ADD);
                vm_writer.write_pop(S_POINTER, 1);
                vm_writer.write_push(S_THAT, 0);
            }
        }
    }
}

int Compilation_engine::compile_expression_list()
{
    int args = 0;
    if (!(tokenizer.get_token_type() == T_SYMBOL && tokenizer.get_symbol() == ')'))
    {
        args++;
        compile_expression();
        while (read_specific_char(','))
        {
            args++;
            compile_expression();
        }
    }
    return args;
}

void Compilation_engine::compile_subroutine_call(string identifier_1)
{
    int16_t num_arg = 0;
    if (!read_specific_char('.')) //method in same class case
    {
        identifier_1 = class_name + '.' + identifier_1;
        vm_writer.write_push(S_POINTER, 0);
        num_arg++;
    }
    else
    {
        string identifier_2 = read_identifier();
        if (identifier_2 == "")
            throw invalid_argument("missing identifier after '.");
        Kind kind = symbol_table.kind_of(identifier_1);
        if (kind != KI_NONE) //method on var
        {
            write_push(identifier_1);
            identifier_1 = symbol_table.type_of(identifier_1) + '.' + identifier_2;
            num_arg++;
        }
        else //function
            identifier_1 = identifier_1 + '.' + identifier_2;
    }
    if (!read_specific_char('('))
        throw invalid_argument("subrouine call missing opening '('");
    num_arg += compile_expression_list();
    if (!read_specific_char(')'))
        throw invalid_argument("subrouine call function missing closing ')'");
    vm_writer.write_call(identifier_1, num_arg);
}

void Compilation_engine::write_push(string identifier)
{
    Kind kind = symbol_table.kind_of(identifier);
    switch (kind)
    {
    case (KI_ARG):
        vm_writer.write_push(S_ARG, symbol_table.index_of(identifier));
        break;
    case (KI_VAR):
        vm_writer.write_push(S_LOCAL, symbol_table.index_of(identifier));
        break;
    case (KI_STATIC):
        vm_writer.write_push(S_STATIC, symbol_table.index_of(identifier));
        break;
    case (KI_FIELD):
        vm_writer.write_push(S_THIS, symbol_table.index_of(identifier));
        break;
    case (KI_NONE):
        throw invalid_argument("cannot push: identifier not in symbol table");
        break;
    }
}
void Compilation_engine::write_pop(string identifier)
{
    Kind kind = symbol_table.kind_of(identifier);
    switch (kind)
    {
    case (KI_ARG):
        vm_writer.write_pop(S_ARG, symbol_table.index_of(identifier));
        break;
    case (KI_VAR):
        vm_writer.write_pop(S_LOCAL, symbol_table.index_of(identifier));
        break;
    case (KI_STATIC):
        vm_writer.write_pop(S_STATIC, symbol_table.index_of(identifier));
        break;
    case (KI_FIELD):
        vm_writer.write_pop(S_THIS, symbol_table.index_of(identifier));
        break;
    case (KI_NONE):
        throw invalid_argument("cannot pop: identifier not in symbol table");
        break;
    }
}

Keyword Compilation_engine::read_keyword()
{
    Keyword keyword = tokenizer.get_keyword();
    if (tokenizer.get_token_type() != T_KEYWORD)
        return K_NONE;
    return keyword;
}

bool Compilation_engine::read_specific_keyword(Keyword required_keyword)
{
    if (!(tokenizer.get_token_type() == T_KEYWORD && tokenizer.get_keyword() == required_keyword))
        return false;
    tokenizer.advance();
    return true;
}

string Compilation_engine::read_type()
{
    Token_type type = tokenizer.get_token_type();
    if (type == T_KEYWORD)
    {
        Keyword keyword = tokenizer.get_keyword();
        switch (keyword)
        {
        case (K_INT):
            tokenizer.advance();
            return "int";
        case (K_CHAR):
            tokenizer.advance();
            return "char";
        case (K_BOOLEAN):
            tokenizer.advance();
            return "boolean";
        default:
            return "";
        }
    }
    else
        return read_identifier();
}

string Compilation_engine::read_identifier()
{
    if (tokenizer.get_token_type() != T_IDENTIFIER)
        return "";
    string return_string = tokenizer.get_idenifier();
    tokenizer.advance();
    return return_string;
}

char Compilation_engine::read_char()
{
    if (tokenizer.get_token_type() != T_SYMBOL)
    {
        return 0;
    }
    return tokenizer.get_symbol();
}

bool Compilation_engine::read_specific_char(char required_char)
{
    if (tokenizer.get_token_type() != T_SYMBOL || tokenizer.get_symbol() != required_char)
    {
        return false;
    }
    tokenizer.advance();
    return true;
}