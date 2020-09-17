#include "Scope.h"
#include <iostream>


using namespace basal;

void Scope::declareVar( string varName, basal::Type type )
{
    if( getVar( varName ).type == UNDECLARED ) // variable is not already declared, declare it
    {
        Variable var( varName, type, ++varNbr ); 
        varArray.push_back( var );
    }
}


Variable Scope::getVar( string varName, unsigned depth )
{
    for( unsigned i=0; i<varArray.size(); i++ )
    {
        if( varName == varArray[i].name ) 
        {
            Variable var = varArray[i];
            var.depth = depth;
            return var;
        }
    }
    // has not been found locally, explore deeper scopes recursivelly
    if( p_parentScope != nullptr ) 
    {
        return p_parentScope->getVar( varName, depth + 1 );
    }

    // no parent scope, return UNDECLARED type var
    Variable var( "#UNDECLARED", UNDECLARED, 0 );
    return var;

}

