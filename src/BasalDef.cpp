#include "BasalDef.h"
#include "lexer.h"

#include <string>
#include <iostream>

using std::string;
using std::cout;
using std::endl;


namespace basal
{

string getStringFromTokenType( TokenType type )
{
    switch( type )
    { 
        case IDENTIFIER:
            return "ID";
        case ADDOP:
            return "ADDOP";
        case MULOP:
            return "MULOP";
        case RELOP:
            return "RELOP";
        case NOT:
            return "NOT";
        case KEYWORD:
            return "KEYWORD";
        case RESERVED_FUNC:
            return "RES_FUNC";
        case QUOTES:
            return "QUOTES";
        case COMMA:
            return "COMMA";
        case AMPERSAND:
            return "AMPERSAND";
        case COLON:
            return "COLON";
        case LPAREN:
            return "LPAREN";
        case RPAREN:
            return "RPARENT";
        case LBRACKET:
            return "LBRACKET";
        case RBRACKET:
            return "RBRACKET";
        case LBRACES:
            return "LBRACES";
        case RBRACES:
            return "RBRACES";
        case DECIMAL_VALUE:
            return "DEC_VAL";
        case HEXA_VALUE:
            return "HEX_VAL";
        case BINARY_VALUE:
            return "BIN_VAL";
        case RESERVED_VALUE:
            return "RES_VAL";
        case TYPE:
            return "TYPE";
        case STRING:
            return "STRING";
        case SPACES:
            return "SPACES";
        case ENDL:
            return "ENDL";
        case STOP:
            return "STOP";
        case EQU:
            return "EQU";
        case UNKNOWN:
            return "UNKNOWN";
        default:
            return "unknown bis";
    }
    return "";
}




    // return the corresponding basal Type
    Type getTypeFromString( string s )
    {
        s = lexer::to_upper( s );
        if     ( s == "VAR" ) return VAR;
        else if( s == "BIN" ) return BIN;
        return TYPE_ERROR ;
    } 

    // return the corresponding string from basal Type
    string getStringFromType( basal::Type type )
    {
        switch( type )
        {
            case VAR:
                return "var";
            case BIN:
                return "bin";
            case UNDECLARED:
                return "undeclared";
            case TYPE_ERROR:
                return "TYPE_ERROR";
            default:
                return "undefined";
        }
    }

}



