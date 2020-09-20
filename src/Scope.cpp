#include "Scope.h"
#include <iostream>

using std::cout;
using std::endl;

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
    if( parentScope != nullptr ) 
    {
        return parentScope->getVar( varName, depth + 1 );
    }
    else
    {
        // no parent scope, return UNDECLARED type var
        Variable var( "#UNDECLARED", UNDECLARED, 0 );
        return var;
    }

}

void Scope::dispDeclVar( unsigned depth )
{
    std::cout << "Scope : " << depth << std::endl;
    for( unsigned i=0; i<varArray.size(); i++ )
    {
        std::cout << i << ": " << varArray[i].name << std::endl;
    }
    if( parentScope != nullptr ) parentScope->dispDeclVar( depth + 1 );
}


