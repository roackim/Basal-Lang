#include "BasalDef.h"

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
                return "op";
            case 2:
                return "relationnal_operator";
            case 3:
                return "keyword";
            case 4:
                return "reserved_identifier";
            case 5:
                return "quotes";
            case 6:
                return "comma";
            case 7:
                return "ampersand";
            case 8:
                return "lparen";
            case 9:
                return "rparen";
            case 10:
                return "lbracket";
            case 11:
                return "rbracket";
            case 12:
                return "lbraces";
            case 13:
                return "rbraces";
            case 14:
                return "decimal_value";
            case 15:
                return "hexa_value";
            case 16:
                return "binary_value";
            case 17:
                return "end of line";
            case 18:
                return "end of file";
            case 19:
                return "string";
            case 20:
                return "spaces";
            case 21:
                return "unkown";
            default:
                return "unkown bis";
        }
        return "ERROR";
    }

}



