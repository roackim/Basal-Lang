#include <string>
#include <iostream>
#include "lexer.h"

using std::string;

namespace lexer
{

    // return lower cased string
    string to_lower( string s )
    {
        string low = s;
        for( unsigned i=0; i<s.length(); i++ )
        {
            if( s[i] >= 'A' and s[i] <= 'Z' )
            {
                low[i] += 32;
            }
        }
        return low;
    }

    // return lower cased string
    string to_upper( string s )
    {
        string low = s;
        for( unsigned i=0; i<s.length(); i++ )
        {
            if( s[i] >= 'a' and s[i] <= 'z' )
            {
                low[i] -= 32;
            }
        }
        return low;
    }
    
    // return true if the string is a basal additive operator
    bool matchADDOP( string op )
    {
        string up = to_upper( op );
        return( op=="+" or op=="-" or up=="OR" or up=="OU" );
    }

    // return true if the string is a basal additive operator
    bool matchMULOP( string op )
    {
        string up = to_upper( op );
        return( op=="*" or op=="/" or op=="^" or op=="%" or op=="." or up=="AND" or up=="ET");
        
    }

    // return true if the string is a basal relationnal operator
    bool matchRELOP( string op )
    {
        return( op=="==" or op=="<" or op==">" or op==">=" or op=="<=" or op=="!=" );
    }

    // return true if the string is a NOT operator
    bool matchNOT( string op )
    {
        op = to_upper( op );
        return( op=="!" or op=="NOT" or op=="NON" );
    }

    // return true if the string is a basal operator
    bool matchOP( string op )
    {
        return( matchADDOP( op ) or matchMULOP( op ) or matchRELOP( op ) );
    }

    // return true if the string is a basal keyword
    bool matchKEYWORD( string op )
    {
        op = to_upper( op );
        return( op=="FUNCTION" or op=="FONCTION" or op=="RETURN" or op=="RENVOYER" or 
                op=="END" or op=="FIN" or op=="IF" or op=="SI" or op=="THEN" or op=="ALORS"  or op=="ELSE" or op=="SINON"
             or op=="FOR" or op=="POUR" or op=="UNTIL" or op=="JUSQUE" or op=="DO" or op=="FAIRE" or op=="WHILE" or op == "TANTQUE" );
    }

    // return true if the string is a basal type
    bool matchTYPE( string op )
    {
        op = to_upper( op );
        return( op=="VAR" or op=="BIN");
    }

    // return true if the string is a basal reserved function
    bool matchRESERVED_FUNC( string op )
    {
        op = to_upper( op ); // non case sensitive op 
        return( op=="DISP" or op=="AFFICHER");  
    }

    // return true if strign literal for boolean
    bool matchRESERVED_VALUE( string op )
    {
        op = to_upper( op );
        return( op=="FALSE" or op=="TRUE" or op=="VRAI" or op=="FAUX" );
    }


    bool isSpace( const char& c )
    {
        return( c == ' ' or c == '\t' );
    }
    bool isNumber( const char& c )
    {
        return( c >= '0' and c <= '9' );
    }
    bool isAlphaNumerical( const char& c )
    {
        return(( c >= 'a' and c <= 'z') or ( c >= 'A' and c <= 'Z' ) or ( c >= '0' and c <= '9'));
    }
    bool isAlpha( const char& c )
    {
        return(( c >= 'a' and c <= 'z') or ( c >= 'A' and c <= 'Z' ));
    }
    bool isHexa( const char& c )
    {
        return( ( c >= '0' and c <= '9' ) or ( c >= 'A' and c <= 'F' ));
    }

    bool matchDecimalValue( const string& s )
    {
        unsigned off = 0; // offset to start the loop approprietly ( skip special char at begining ex : -123 )

        if( s[0]  == '-' ) off = 1;
        if( not (s.length() > off )) return false; // must have atleast one number
        for( unsigned i=off; i < s.length(); i++ )
        {
            if( not isNumber( s[i] )) return false; // must have atleast one number
        }
        return true;
    }

    bool matchHexaValue( const string& s )
    {
        unsigned off = 0;

        if( s[0]  == '0' and s[1] == 'x') off = 2;
        else return false;
        if( not (s.length() > off )) return false; // must have atleast one number
        for( unsigned i=off; i < s.length(); i++ )
        {
            if( not isHexa( s[i] )) return false; // must have atleast one number
        }
        return true;
    }

    bool matchBinValue( const string& s )
    {
        unsigned off = 0;

        if( s[0]  == '0' and s[1] == 'b') off = 2;
        else return false;
        if( not (s.length() > off )) return false; // must have atleast one number
        for( unsigned i=off; i < s.length(); i++ )
        {
            if(( s[i] != '0') and ( s[i] != '1')) return false; // must have atleast one number
        }
        return true;
    }

    bool matchCharValue( const string& s )
    {
        if( s[0] != '\'' )
            return false;
        else
        {
            if( s.length() == 3 )
            {
                if( s[2] == '\'' ) 
                    return true;
                return false;
            }
            else if( s.length() == 4 )
            {
                if( s[3] == '\'' )
                    if( s[1] == '\\' )
                        return true;
                return false;
            }
            else
                return false;
        }
    }

    bool matchIdentifier( const string& s )
    {

        if( not isAlpha( s[0] ) ) return false; // must start with [a-Z] 
        for( unsigned i=0; i < s.length(); i++ )
        {
            if( not (isAlphaNumerical( s[i] ) or s[i] == '_') ) return false; // must have atleast one number
        }
        return true;
    }



    // can't have nested functions in C++ :( , this is a work-around
    void endWord( vector<string>& words, string& word ) // push word if not empty
    {
        if( word == "" ) return;
        words.push_back( word );
        word = "";  // empty variable for the next word
    }

    bool isEscaped( const string& line, const uint32_t& i )
    {
        return( i > 0 and line[i-1] == '\\');
    }
    
    // helper function for splitLine
    bool matchOneLetterOP( char op )
    {
        return( op=='='  or op=='<'   or op=='>' or op=='>' or op=='<' or op=='!' 
             or op=='+'  or op=='-' or op=='/'   or op=='*'   or op=='^'  or op=='%' );
    }

    // split a string into words, stored in a vector
    vector<string> splitLine( string line, bool tokenizeSpaces )
    {
        vector<string> words;
        string word = "";

        bool esc = false; // true if the previous char was '\'
        bool quotes = false; // wether or not the current char is part of a quotation ex: DISP("HelloWorld!")
                             //                                                                       ^--------- quotes would be true here 
        for( uint32_t i=0; i < line.length(); i++) // every char of the string
        {
            char c = line[i];

            if( c == '\"' )
            {
                if( not esc )
                {
                    endWord( words, word ); 
                    quotes = not quotes;
                    word += line[i];
                    endWord( words, word );
                }
                else 
                    word += line[i];
                continue;
            }
            else if( quotes == true and c != '\\' )
            {
                word += line[i];
                continue;
            }
            else if( isSpace( line[i]) and quotes == false )
            {    
                endWord( words, word );
                if( tokenizeSpaces ) word += line[i];
                while( isSpace( line[i+1]) ) 
                {
                    if( tokenizeSpaces ) word += line[i+1];
                    i++;    
                }
                if( tokenizeSpaces ) endWord( words, word );
                continue;
            }
            else if( c==',' or c=='&' or c=='(' or c==')' or c=='[' or c==']' or c=='{' or c=='}' or c=='.' or c==';' or c==':')
            {
                if( quotes ) word += line[i];
                else
                {
                    endWord( words, word );
                    word += line[i];
                    endWord( words, word );
                }
                continue;
            }
            else if( line[i] == '#') // discard the rest of the line if the char is not escaped
            {
                if( esc ) word += line[i]; // continue normally
                else 
                { 
                    endWord( words, word );
                    break;      // stop processing the line
                }
                continue;
            }
            else if( line[i] == '\\' ) 
            {
                if( esc == false ) esc = true;       // chain escapement ex : DISP("\\\\")
                else if( esc == true )
                {
                    word += line[i];
                    esc = false;
                } 
            }
            else if( matchOneLetterOP( c ) and not quotes)
            {
                endWord( words, word );
                if( line[i+1] == '=' )
                {
                    word += line[i];
                    string s{ c }; s += line[i+1];
                    if( matchOP( s ))
                    {
                        i++;
                        word += line[i];
                        endWord( words, word );
                        continue;
                    }
                    continue;
                }
                else
                {
                    word += line[i];
                    endWord( words, word );
                    continue;
                }
            }
            else // default case
            { 
                esc = false;
                word += line[i];
                continue;
            }
        }
        endWord( words, word );
        word += "#"; // end of line token, use '#' char, so ';' can be used as a separator, not incrementing line count
        endWord( words, word );
        
        return words;
    }

    token tokenizeOneWord( string txt )
    {
        static bool quotes = false;
        using namespace basal;
        TokenType type = UNKNOWN;
        if     ( txt == "," ) type = COMMA;
        else if( txt == "\""){ type = QUOTES; quotes = not quotes; }
        else if( txt == ";" or txt == "#" ) type = ENDL;
        else if( txt == "&" ) type = AMPERSAND;
        else if( txt == ":" ) type = COLON;
        else if( txt == "(" ) type = LPAREN;
        else if( txt == ")" ) type = RPAREN;
        else if( txt == "[" ) type = LBRACKET;
        else if( txt == "]" ) type = RBRACKET;
        else if( txt == "{" ) type = LBRACES;
        else if( txt == "}" ) type = RBRACES;
        else if( txt == "=" ) type = EQU;
        else if( matchRELOP( txt ))             type = RELOP;           // try to match relationnal operators
        else if( matchADDOP( txt ))             type = ADDOP;           // try to match additive operators
        else if( matchMULOP( txt ))             type = MULOP;           // try to match additive operators
        else if( matchNOT( txt ))               type = NOT;             // try to match not operator
        else if( matchRESERVED_FUNC( txt ))     type = RESERVED_FUNC;   // try to match reserved functions
        else if( matchRESERVED_VALUE( txt ))    type = RESERVED_VALUE;  // try to match reserved functions
        else if( matchTYPE( txt ))              type = TYPE;            // try to match basal type declaration
        else if( matchKEYWORD( txt ))           type = KEYWORD;         // try to match basal keywords
        else if( matchDecimalValue( txt ))      type = DECIMAL_VALUE;   // try to match decimal values
        else if( matchHexaValue( txt ))         type = HEXA_VALUE;      // try to match hexa values
        else if( matchBinValue( txt ))          type = BINARY_VALUE;    // try to match binary values
        else if( matchIdentifier( txt))         type = IDENTIFIER;      // try to match label call ex: jump Hello_World_Proc
        else if( quotes )                       type = STRING;
        else if( isSpace( txt[0] ))             type = SPACES;

        token ret( txt, type );
        return( ret ); 
    }
}
