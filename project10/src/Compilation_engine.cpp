#include "Compilation_engine.h"

Compilation_engine::Compilation_engine(string input_filename, string output_filename)
    : tokenizer{input_filename}, out_stream{output_filename} {}

void Compilation_engine::run()
{
    while (tokenizer.is_good())
        compile_class();
}

void Compilation_engine::compile_class()
{
    if (!read_structure_opening("class", {K_CLASS}))
        throw invalid_argument("found token outside class");
    if (!read_identifier())
        throw invalid_argument("class missing identifier");
    if (!read_specific_char('{'))
        throw invalid_argument("class missing opening '{'");

    while (compile_class_var_dec())
        ;
    while (compile_subroutine())
        ;
    if (!read_specific_char('}'))
        throw invalid_argument("class missing closing '}'");
    out_stream << "</class>\n";
}

bool Compilation_engine::compile_class_var_dec()
{
    if (!read_structure_opening("classVarDec", {K_STATIC, K_FIELD}))
        return false;
    if (!read_type())
        throw invalid_argument("class var declaration missing type");
    if (!read_identifier())
        throw invalid_argument("class var declaration missing identifier");
    while (read_specific_char(','))
        if (!read_identifier())
            throw invalid_argument("class var declaration has extra ','");
    if (!read_specific_char(';'))
        throw invalid_argument("declared class var must be terminated by ';'");
    out_stream << "</classVarDec>\n";
    return true;
}

bool Compilation_engine::compile_subroutine()
{
    if (!read_structure_opening("subroutineDec", {K_CONSTRUCTOR, K_FUNCTION, K_METHOD}))
        return false;
    if (!(read_type() || read_specific_keyword(K_VOID)))
        throw invalid_argument("subroutine must have return var type");
    if (!read_identifier())
        throw invalid_argument("subroutine missing identifier");
    if (!read_specific_char('('))
        throw invalid_argument("subroutine missing opening '(");
    compile_parameter_list();
    if (!read_specific_char(')'))
        throw invalid_argument("subroutine missing closing '(");
    out_stream << "<subroutineBody>\n";
    if (!read_specific_char('{'))
        throw invalid_argument("subroutine missing opening '{");
    while (compile_var_dec())
        ;
    compile_statements();
    if (!read_specific_char('}'))
        throw invalid_argument("subroutine missing closing '}");
    out_stream << "</subroutineBody>\n";
    out_stream << "</subroutineDec>\n";
    return true;
}

void Compilation_engine::compile_parameter_list()
{
    out_stream << "<parameterList>\n";
    if (read_type())
    {
        if (!read_identifier())
            throw invalid_argument("parameter missing identifier");
        while (read_specific_char(','))
        {
            read_type();
            if (!read_identifier())
                throw invalid_argument("parameter list has extra ','");
        }
    }
    out_stream << "</parameterList>\n";
}

bool Compilation_engine::compile_var_dec()
{
    if (!read_structure_opening("varDec", {K_VAR}))
        return false;
    if (!read_type())
        throw invalid_argument("var declaration missing type");
    if (!read_identifier())
        throw invalid_argument("var declaration missing identifier");
    while (read_specific_char(','))
        if (!read_identifier())
            throw invalid_argument("var declaration has extra ','");
    if (!read_specific_char(';'))
        throw invalid_argument("declared var must be terminated by ';'");
    out_stream << "</varDec>\n";
    return true;
}

void Compilation_engine::compile_statements()
{
    out_stream << "<statements>\n";
    while (compile_statement())
        ;
    out_stream << "</statements>\n";
}

bool Compilation_engine::compile_do()
{
    if (!read_structure_opening("doStatement", {K_DO}))
        return false;
    if (!read_identifier())
        throw invalid_argument("do statement missing function name");
    if (read_specific_char('.'))
        if (!read_identifier())
            throw invalid_argument("do statement missing function name after '.'");
    if (!read_specific_char('('))
        throw invalid_argument("do statement function missing opening '('");
    compile_expression_list();
    if (!read_specific_char(')'))
        throw invalid_argument("do statement function missing closing ')'");
    if (!read_specific_char(';'))
        throw invalid_argument("do statement function missing terminating ';'");
    out_stream << "</doStatement>\n";
    return true;
}

bool Compilation_engine::compile_let()
{
    if (!read_structure_opening("letStatement", {K_LET}))
        return false;
    if (!read_identifier())
        throw invalid_argument("let statement missing identifier");
    if (read_specific_char('['))
    {
        compile_expression();
        if (!read_specific_char(']'))
            throw invalid_argument("let statement missing closing ']'");
    }
    if (!read_specific_char('='))
        throw invalid_argument("let statement missing '='");
    compile_expression();
    ;
    if (!read_specific_char(';'))
        throw invalid_argument("let statement missing terminating ';'");
    out_stream << "</letStatement>\n";
    return true;
}
bool Compilation_engine::compile_while()
{
    if (!read_structure_opening("whileStatement", {K_WHILE}))
        return false;
    if (!read_specific_char('('))
        throw invalid_argument("while statement missing opening '('");
    compile_expression();
    if (!read_specific_char(')'))
        throw invalid_argument("while statement missing closing ')'");
    if (!read_specific_char('{'))
        throw invalid_argument("while statement missing opening '{'");
    compile_statements();
    if (!read_specific_char('}'))
        throw invalid_argument("while statement missing closing '}'");
    out_stream << "</whileStatement>\n";
    return true;
}

bool Compilation_engine::compile_return()
{
    if (!read_structure_opening("returnStatement", {K_RETURN}))
        return false;
    if (!read_specific_char(';'))
    {
        compile_expression();
        if (!read_specific_char(';'))
            throw invalid_argument("return statement missing terminating ;");
    }
    out_stream << "</returnStatement>\n";
    return true;
}

bool Compilation_engine::compile_if()
{
    if (!read_structure_opening("ifStatement", {K_IF}))
        return false;
    if (!read_specific_char('('))
        throw invalid_argument("if statement missing opening '('");
    compile_expression();
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
        compile_statements();
        if (!read_specific_char('}'))
            throw invalid_argument("else statement missing closing '}'");
    }
    out_stream << "</ifStatement>\n";
    return true;
}

void Compilation_engine::compile_expression()
{
    out_stream << "<expression>\n";
    compile_term();
    while (read_char({'+', '-', '*', '/', '&', '|', '<', '>', '='}))
        compile_term();
    out_stream << "</expression>\n";
}

void Compilation_engine::compile_term()
{
    out_stream << "<term>\n";
    switch (tokenizer.get_token_type())
    {
    case (T_INT_CONST):
        print_int_val();
        tokenizer.advance();
        break;
    case (T_STRING_CONST):
        print_string_val();
        tokenizer.advance();
        break;
    case (T_KEYWORD):
    {
        Keyword keyword = tokenizer.get_keyword();
        if (keyword == K_TRUE || keyword == K_FALSE || keyword == K_NULL || keyword == K_THIS)
        {
            print_keyword();
            tokenizer.advance();
            break;
        }
        else
            throw invalid_argument("disallowed keyword in term");
    }
    case (T_SYMBOL):
    {
        char symbol = tokenizer.get_symbol();
        if (symbol == '-' || symbol == '~')
        {
            print_symbol();
            tokenizer.advance();
            compile_term();
            break;
        }
        else if (symbol == '(')
        {
            print_symbol();
            tokenizer.advance();
            compile_expression();
            if (!read_specific_char(')'))
                throw invalid_argument("term missing closing ')'");
            break;
        }
        else
            throw invalid_argument("disallowed symbol in term");
    }
    case (T_IDENTIFIER):
        read_identifier();
        if (read_specific_char('.')) //subroutine call case
        {
            if (!read_identifier())
                throw invalid_argument("subroutine call missing function name after '.'");
            if (!read_specific_char('('))
                throw invalid_argument("subroutine call function missing opening '('");
            compile_expression_list();
            if (!read_specific_char(')'))
                throw invalid_argument("subroutine call function missing closing ')'");
        }
        else if (read_specific_char('[')) //var[] case
        {
            compile_expression();
            if (!read_specific_char(']'))
                throw invalid_argument("var in temp missing expression in [...]");
        }
    }
    out_stream << "</term>\n";
}

void Compilation_engine::compile_expression_list()
{
    out_stream << "<expressionList>\n";
    if (!(tokenizer.get_token_type() == T_SYMBOL && tokenizer.get_symbol() == ')'))
    {
        compile_expression();
        while (read_specific_char(','))
            compile_expression();
    }
    out_stream << "</expressionList>\n";
}

bool Compilation_engine::compile_statement()
{
    return (compile_let() || compile_if() || compile_while() || compile_do() || compile_return());
}

bool Compilation_engine::read_structure_opening(string structure_name, set<Keyword> keyword_list)
{
    Keyword keyword = tokenizer.get_keyword();
    if (!(tokenizer.get_token_type() == T_KEYWORD && keyword_list.find(keyword) != keyword_list.end()))
        return false;
    out_stream << "<" << structure_name << ">\n";
    print_keyword();
    tokenizer.advance();
    return true;
}

bool Compilation_engine::read_specific_keyword(Keyword required_keyword)
{
    if (!(tokenizer.get_token_type() == T_KEYWORD && tokenizer.get_keyword() == required_keyword))
        return false;
    print_keyword();
    tokenizer.advance();
    return true;
}

bool Compilation_engine::read_type()
{
    Token_type type = tokenizer.get_token_type();
    if (type == T_KEYWORD)
    {
        Keyword keyword = tokenizer.get_keyword();
        if (keyword == K_INT || keyword == K_CHAR || keyword == K_BOOLEAN)
            print_keyword();
        else
            return false;
    }
    else if (type == T_IDENTIFIER)
        print_identifier();
    else
        return false;
    tokenizer.advance();
    return true;
}

bool Compilation_engine::read_identifier()
{
    if (tokenizer.get_token_type() != T_IDENTIFIER)
        return false;
    print_identifier();
    tokenizer.advance();
    return true;
}

bool Compilation_engine::read_char(set<char> char_set)
{
    if (tokenizer.get_token_type() != T_SYMBOL || char_set.find(tokenizer.get_symbol()) == char_set.end())
    {
        return false;
    }
    print_symbol();
    tokenizer.advance();
    return true;
}

bool Compilation_engine::read_specific_char(char required_char)
{
    if (tokenizer.get_token_type() != T_SYMBOL || tokenizer.get_symbol() != required_char)
    {
        return false;
    }
    print_symbol();
    tokenizer.advance();
    return true;
}

//output functions
void Compilation_engine::print_identifier()
{
    out_stream << "<identifier> " << tokenizer.get_idenifier() << " </identifier>\n";
}

void Compilation_engine::print_keyword()
{
    out_stream << "<keyword> ";
    Keyword keyword = tokenizer.get_keyword();
    switch (keyword)
    {
    case (K_CLASS):
        out_stream << "class";
        break;
    case (K_METHOD):
        out_stream << "method";
        break;
    case (K_FUNCTION):
        out_stream << "function";
        break;
    case (K_CONSTRUCTOR):
        out_stream << "constructor";
        break;
    case (K_INT):
        out_stream << "int";
        break;
    case (K_BOOLEAN):
        out_stream << "boolean";
        break;
    case (K_CHAR):
        out_stream << "char";
        break;
    case (K_VOID):
        out_stream << "void";
        break;
    case (K_VAR):
        out_stream << "var";
        break;
    case (K_STATIC):
        out_stream << "static";
        break;
    case (K_FIELD):
        out_stream << "field";
        break;
    case (K_LET):
        out_stream << "let";
        break;
    case (K_DO):
        out_stream << "do";
        break;
    case (K_IF):
        out_stream << "if";
        break;
    case (K_ELSE):
        out_stream << "else";
        break;
    case (K_WHILE):
        out_stream << "while";
        break;
    case (K_RETURN):
        out_stream << "return";
        break;
    case (K_TRUE):
        out_stream << "true";
        break;
    case (K_FALSE):
        out_stream << "false";
        break;
    case (K_NULL):
        out_stream << "null";
        break;
    case (K_THIS):
        out_stream << "this";
        break;
    }
    out_stream << " </keyword>\n";
}

void Compilation_engine::print_symbol()
{
    if (tokenizer.get_symbol() == '<')
    {
        out_stream << "<symbol> "
                   << "&lt;"
                   << " </symbol>\n";
    }
    else if (tokenizer.get_symbol() == '>')
    {
        out_stream << "<symbol> "
                   << "&gt;"
                   << " </symbol>\n";
    }
    else if (tokenizer.get_symbol() == '&')
    {
        out_stream << "<symbol> "
                   << "&amp;"
                   << " </symbol>\n";
    }
    else
    {
        out_stream << "<symbol> " << tokenizer.get_symbol() << " </symbol>\n";
    }
}
void Compilation_engine::print_int_val()
{
    out_stream << "<integerConstant> " << tokenizer.get_int_val() << " </integerConstant>\n";
}
void Compilation_engine::print_string_val()
{
    out_stream << "<stringConstant> " << tokenizer.get_string_val() << " </stringConstant>\n";
}