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
        NOT,
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
        RESERVED_VALUE,
        TYPE,
        STRING,
        SPACES,
        ENDL,
        STOP,
        EQU,
        UNKNOWN
    };
    enum Type   // basal types // TODO add more types ?
    {
        VAR,
        BOOL,
        UNDECLARED,
        TYPE_ERROR
    };


    string getStringFromTokenType( TokenType type );

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
