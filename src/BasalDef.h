#pragma once

#include <string>
#include <iostream>

using std::string;


namespace basal
{

    enum TokenType   // types of tokens
    {
        IDENTIFIER,         // identifier
        ADDOP,              // additive operator
        MULOP,              // multiplicative operator 
        RELOP,              // relationnal operator
        KEYWORD,            // keyword
        RESERVED_FUNC,           // reserved functions
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
        STRING,
        SPACES,
        TYPE,
        RESERVED_VALUE,
        UNKNOWN
    };
    enum Type   // basal types // TODO add more types ?
    {
        VAR,
        BIN,
        UNDECLARED,
        VARTYPEERROR
    };

    // helper function, allow to get string from enum
    string getTokenTypeStr( TokenType type );

    // return the corresponding basal Type
    Type getTypeFromString( string s );

    // return the corresponding string from basal Type
    string getStringFromType( basal::Type type );
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
