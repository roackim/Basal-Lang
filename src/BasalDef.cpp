#include "BasalDef.h"
#include "lexer.h"

#include <string>
#include <iostream>

using std::string;
using std::cout;
using std::endl;


namespace basal
{

    // return the corresponding basal Type
    Type getTypeFromString( string s )
    {
        s = lexer::to_upper( s );
        if( s == "VAR" ) return VAR;
        else if( s == "BOOL" ) return BOOL;
        return TYPE_ERROR ;
    } 

    // return the corresponding string from basal Type
    string getStringFromType( basal::Type type )
    {
        switch( type )
        {
            case VAR:
                return "var";
            case BOOL:
                return "bool";
            case UNDECLARED:
                return "undeclared";
            case TYPE_ERROR:
                return "TYPE_ERROR";
            default:
                return "undefined";
        }
    }

}



