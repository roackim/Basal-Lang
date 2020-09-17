#pragma once

#include <map>
#include <string>

using std::map;
using std::string;

class Scope
{
public:
    Scope( Scope* p ): parent( p ){ };
    bool isDeclared( string var ); // return true if the var is known in this scope, or any parent scope
    void declareVar( string var, unsigned offset );

private:
    map<string, unsigned> declared_variables; // contains the local variables of the scope (including parameters)
    unsigned varNbr = 0;
    Scope* parent = nullptr;
};
