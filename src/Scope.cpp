#include "Scope.h"
#include <iostream>

bool Scope::isDeclared( string varName )
{
    Scope* p_scope = this;      // point successivly to every opened scope until varName has been found or reached global scope

    while( p_scope != nullptr )
    {
        if( p_scope->isDeclaredLocally( varName) ) 
        {
            return true;        // variable has been declared
        }
        p_scope = p_scope->parent;  // go to parent scope
    }
    return false; // variable has not been declared 
}

void Scope::declareVar( string varName, basal::Type type )
{
    if( isDeclared( varName) ) std::cerr << "SHOULD BE CHECKED BEFORE: varNamealready declared" << std::endl;
    else // variable is not already declared, declare it
    {
        Variable var( varName, type, varNbr++ ); 
        declaredVariables.push_back( var );
    }
}

bool Scope::isDeclaredLocally( string varName )
{
    for( unsigned i=0; i<declaredVariables.size(); i++ )
    {
        if( varName == declaredVariables[i].name ) return true;
    }
    return false;
}
