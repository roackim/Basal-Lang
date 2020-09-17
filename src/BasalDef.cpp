#include "BasalDef.h"
#include "lexer.h"

#include <string>
#include <iostream>

using std::string;
using std::cout;
using std::endl;


namespace basal
{

    // helper function, allow to get string from enum
    string getTokenTypeStr( TokenType type )
    {
        switch( type )
        {
            case 0:
                return "identifier";
            case 1:
                return "add_op";
            case 2:
                return "mul_op";
            case 3:
                return "rel_op";
            case 4:
                return "keyword";
            case 5:
                return "reserved_identifier";
            case 6:
                return "quotes";
            case 7:
                return "comma";
            case 8:
                return "ampersand";
            case 9:
                return "lparen";
            case 10:
                return "rparen";
            case 11:
                return "lbracket";
            case 12:
                return "rbracket";
            case 13:
                return "lbraces";
            case 14:
                return "rbraces";
            case 15:
                return "decimal_value";
            case 16:
                return "hexa_value";
            case 17:
                return "binary_value";
            case 18:
                return "end_of_line";
            case 19:
                return "end_of_file";
            case 20:
                return "string";
            case 21:
                return "spaces";
            case 22:
                return "type";
            case 23:
                return "reserved_value";
            case 24:
                return "unkown";
            default:
                return "unkown_bis";
        }
        return "ERROR";
    }

    // return the corresponding basal Type
    Type getTypeFromString( string s )
    {
        s = lexer::to_upper( s );
        if( s == "VAR" ) return VAR;
        else if( s == "BIN" ) return BIN;
        return VARTYPEERROR ;
    } 

    // return the corresponding string from basal Type
    string getStringFromType( basal::Type type )
    {
        switch( type )
        {
            case VAR:
                return "int";
            case BIN:
                return "bin";
            case UNDECLARED:
                return "undeclared";
            case VARTYPEERROR:
                return "VARTYPEERROR:not supposed to happen";
            default:
                return "WTF";
        }
    }

}



