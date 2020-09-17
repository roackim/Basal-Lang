#pragma once

#include <string>
#include <iostream>

using std::string;


namespace basal
{

    enum TokenType   // types of tokens
    {
        IDENTIFIER,         // identifier
        OP,                 // operator
        KEYWORD,            // keyword
        RESERVED,           // reserved functions
        QUOTES,
        COMMA,
        AMPERSAND,
        LPAREN,
        RPAREN,
        LBRACKET,
        RBRACKET,
        LBRACES,
        RBRACES,
        DECIMAL_VALUE,      //  |
        HEXA_VALUE,         //   > values
        BINARY_VALUE,       //  |
        ENDL,
        STOP,
        UNKNOWN
    };

    // helper function, allow to get string from enum
    string getTokenTypeStr( TokenType type );
}

namespace lexer
{
    class token // contains the type and the text 
    {
    public:
        string text;
        basal::TokenType type = basal::UNKNOWN;
        token( const string& s, basal::TokenType t )
        : text( s )
        , type( t ) { }
        token()
        {
        }
    };
}
