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
    Variable( string p_name, basal::Type p_type, unsigned p_offset)
    : name( p_name )
    , type( p_type )
    , offset( p_offset )
    , depth( 0 ){}; 

    string name;            // variable name
    basal::Type type;       // variable type
    unsigned offset;        // position in stack relative to scope position
    unsigned depth;         // how many scope ago was the var declared ( must
};




class Scope
{
public:
    Scope( Scope* p ): p_parentScope( p ){ };
    bool isDeclared( string varName ); // return true if the var is known in this scope, or any parentScope scope
    void declareVar( string varName, basal::Type type );
    Variable getVar( string varName, unsigned depth = 0 );

private:
    bool isDeclaredLocally( string varName );
    vector<Variable> varArray;
    unsigned varNbr = 0;
    Scope* p_parentScope = nullptr;
};


