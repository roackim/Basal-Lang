#pragma once
#include <string>
#include <vector>
#include "BasalDef.h"

using std::string;
using std::vector;

namespace lexer
{
    // lower a string, allow for non-case-sensitive OP 
    string to_lower( string s );

    // uppercase a string, allow for non-case-sensitive OP 
    string to_upper( string s );

    // return true if the string is a basal additive operator
    bool matchADDOP( string op );

    // return true if the string is a basal additive operator
    bool matchMULOP( string op );

    // return true if the string is a basal relationnal operator
    bool matchRELOP( string op );

    // return true if the string is a NOT operator
    bool matchNOT( string op );

    // return true if the string is a basal operator
    bool matchOP( string op );

    // return true if the string is a basal keyword
    bool matchKEYWORD( string op );

    // return true if the string is a basal type
    bool matchTYPE( string op );

    // return true if the string is a basal reserved function
    bool matchRESERVED_FUNC( string op );

    // return true if strign literal for boolean
    bool matchRESERVED_VALUE( string op );


    // helpers functions used to replaced regex, because of terrible performance (i dont know why std::regex was so slow, i put in parser.cpp/h the code that replace the regex)
    bool isSpace( const char& c );
    bool isNumber( const char& c );
    bool isAlphaNumerical( const char& c );
    bool isAlpha( const char& c );
    bool isHexa( const char& c );
    // return true if the string correctly match, false ohterwise, allow for cascade matching
    bool matchDecimalValue( const string& s );
    bool matchHexaValue( const string& s );
    bool matchBinValue( const string& s );
    bool matchCharValue( const string& s );
    bool matchIdentifier( const string& s );

    // Helper functions for splitLine function
    void endWord( vector<string>& words, string& word ); // push word if not empty
    bool isEscaped( const string& line, const uint32_t& i );
    bool matchOneLetterOP( char op );

    // split a string with a delimiter 
    vector<string> splitLine( string line, bool tokenizeSpaces=false );
    token tokenizeOneWord( string txt );


}


