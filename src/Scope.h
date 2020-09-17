#pragma once
#include "BasalDef.h"
#include <map>
#include <string>
#include <vector>

using std::map;
using std::string;
using std::vector;


struct Variable
{
    Variable( string Name, basal::Type Type, unsigned off )
    : name( Name )
    , type( Type )
    , offset( off ){}; 

    string name;     // variable name
    basal::Type type;       // variable type
    unsigned offset;        // position in stack relative to scope position
};




class Scope
{
public:
    Scope( Scope* p ): parent( p ){ };
    bool isDeclared( string varName ); // return true if the var is known in this scope, or any parent scope
    void declareVar( string varName, basal::Type type );

private:
    bool isDeclaredLocally( string varName );
    vector<Variable> declaredVariables;
    unsigned varNbr = 0;
    Scope* parent = nullptr;
};


