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
        case DOT:
            return "DOT";
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
        case FLOAT_VALUE:
            return "FLOAT_VAL";
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
    Type getTypeFromString( string op )
    {
        op = lexer::to_upper( op );
        if     ( op=="INT" or op=="INTEGER" or op=="ENT" or op=="ENTIER" ) return INT;
        else if( op=="BIN" or op=="BINARY" or op=="BINAIRE" ) return BIN;
        else if( op=="DEC" or op == "DECIMAL" ) return FLOAT;
        return TYPE_ERROR ;
    } 

}



