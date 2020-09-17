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
                return "keyword";
            case 3:
                return "reserved_identifier";
            case 4:
                return "quotes";
            case 5:
                return "comma";
            case 6:
                return "ampersand";
            case 7:
                return "lparen";
            case 8:
                return "rparen";
            case 9:
                return "lbracket";
            case 10:
                return "rbracket";
            case 11:
                return "lbraces";
            case 12:
                return "rbraces";
            case 13:
                return "decimal_value";
            case 14:
                return "hexa_value";
            case 15:
                return "binary_value";
            case 16:
                return "end of line";
            case 17:
                return "end of file";
            case 18:
                return "unkown";
            default:
                return "unkown bis";
        }
        return "ERROR";
    }

}



