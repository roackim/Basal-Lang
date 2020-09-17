#include "Scope.h"
#include <iostream>

bool Scope::isDeclared( string var )
{
    Scope* scope = this;
    // label is already defined, throw error
    while( scope != nullptr )
    {
        if( declared_variables.count( var ) == 1 ) 
        {
            return true; // variable has been declared
        }
        scope = scope->parent; // go to parent scope
    }
    return false; // variable has not been declared 
}

void Scope::declareVar( string var, unsigned offset )
{
    if( isDeclared( var ) ) std::cerr << "SHOULD NOT HAPPEN: var already declared" << std::endl;
    else // variable is not already declared, declare it
    {
        declared_variables.insert( std::pair<string, unsigned>( var, offset ));
        varNbr += 1;
    }
}
