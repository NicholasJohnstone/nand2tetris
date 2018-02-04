#include "Symbol_table.h"

void Symbol_table::start_subroutine()
{
    symbol_count[KI_ARG] = 0;
    symbol_count[KI_VAR] = 0;
    method_table.clear();
}

void Symbol_table::define(string name, string type, Kind kind)
{
    pair<map<string, tuple<string, Kind, int>>::iterator, bool> ret;
    switch (kind)
    {
    case (KI_STATIC):
    case (KI_FIELD):
        ret = class_table.insert({name, tuple<string, Kind, int>(type, kind, symbol_count[kind])});
        symbol_count[kind]++;
        break;
    case (KI_ARG):
    case (KI_VAR):
        ret = method_table.insert({name, tuple<string, Kind, int>(type, kind, symbol_count[kind])});
        symbol_count[kind]++;
        break;
    case (KI_NONE):
        throw invalid_argument("cannot define symbol with no type");
        break;
    }
    if (ret.second == false)
    {
        throw invalid_argument("cannot define a symbol twice in the same scope");
    }
}

int Symbol_table::var_count(Kind kind)
{
    return symbol_count[kind];
}

Kind Symbol_table::kind_of(string name)
{
    map<string, tuple<string, Kind, int>>::iterator iter;
    iter = method_table.find(name);
    if (iter != method_table.end())
        return get<1>((*iter).second);
    iter = class_table.find(name);
    if (iter != class_table.end())
        return get<1>((*iter).second);
    return KI_NONE;
}
string Symbol_table::type_of(string name)
{
    map<string, tuple<string, Kind, int>>::iterator iter;
    iter = method_table.find(name);
    if (iter != method_table.end())
        return get<0>((*iter).second);
    iter = class_table.find(name);
    if (iter != class_table.end())
        return get<0>((*iter).second);
    return "";
}
int Symbol_table::index_of(string name)
{
    map<string, tuple<string, Kind, int>>::iterator iter;
    iter = method_table.find(name);
    if (iter != method_table.end())
        return get<2>((*iter).second);
    iter = class_table.find(name);
    if (iter != class_table.end())
        return get<2>((*iter).second);
    return -1;
}