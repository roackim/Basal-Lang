#include <iterator>
#include <fstream>
#include <cmath>
#include <vector>
#include "Compiler.h"
#include "lexer.h"

 
namespace basal
{
    // compile basal file to basal assembly
    bool Compiler::compile( string fileName )
    {
        if( !loadAndTokenize( fileName )) // error while tokenizing
            return false;

        // if( !tokens.empty() ) //  TODO change
        // {
        //     
        //     while( parseOneInstr() )
        //     {
        //         if( !readEndl() )  // expect one instruction per line
        //         {
        //             break; // stop compilation
        //         }
        //     }         
        //     return true;
        // }
        // else
        // {
        //     cerr << "/!\\ Error while assembling : No instruction found in file '" << fileName << "'." << endl;
        //     return false;
        // }
        return true;
    }

    // better error message for compilation
    bool Compiler::compileError( std::string message )
    {

        std::cerr << "\n/!\\ Error line " << lineNbr << ": " << message << "." << endl;
    
        string line = "";
        unsigned marker = 0;
        unsigned i = j;
        if( current.type == ENDL ) i--;
        while( tokens[ i ].type != ENDL ) i--;
        i++;
        while( tokens[ i ].type != ENDL )
        {
            line += tokens[i].text + " ";

            if( i == j )    
            {
                marker = line.length()+1 - current.text.length() / 2;
            }
            i++;
        }    
        if( i == j and current.type == ENDL )    
        {
            marker = line.length()+3 - current.text.length() / 2;
        }

        cout << "--> " << line << endl ;
        for( unsigned k=0; k<=marker; k++)
            cout << " ";
        cout << "^" << endl;

        exit(-1); // subject to change
        return false;
    }

    // increment j and reassign token t
    bool Compiler::readToken( void )
    {
        if( j < tokens.size() - 2 )
        {    
            current = tokens[ ++j ];
            return true;         
        }
        else
        {
            current = token("End of file", STOP );
            return false;
        }
    }

    // curent token must be a ENDL, compileError and return false otherwise
    bool Compiler::readEndl( void )
    {
        if( current.type == ENDL )
        {
            readToken();
            if( current.text == "#" ) lineNbr++; // do not increment line Nbr if used ';' to separate 2 instructions
            return true;
        }
        else
        {
            compileError("Expected End of line after instruction");
            return false;
        }
    }

    // current token must be a comma, compileError and return false otherwise
    bool Compiler::readComma( void )
    {
        if( tokens[ j ].type == COMMA )
        {
            readToken();
            return true;
        }
        else
        {
            compileError("Expected a comma, not '" + current.text + "'");
            return false;
        }
    }

    // parse decimals, binary and hexadecimal value, return a uint16 encoded value, and call readToken()
    uint16_t Compiler::parseValue( void )
    {
        string value = current.text;
        if( current.type == DECIMAL_VALUE )
        {
            int32_t i = atoi( value.c_str() );
            if( i > 65535 or i < -32768 )
            {
                compileError( "Value '" + current.text + "' is too big to be encoded.\n\trange: [0, 65535] or [-32768, 32767]" );
                return false;
            }
            readToken();
            return static_cast<uint16_t>( i );
        }
        else if( current.type == BINARY_VALUE )
        {
            value = value.substr( 2, value.length() - 1 ); // remove the base before number eg : 0b0101 -> 0101
            if( value.length() > 16 )
            {
                compileError( "Value '" + current.text + "' is too big to be encoded.\n\trange: [0, 65535] or [-32768, 32767]" );
                return false;
            }
            long int i = std::stol( value.c_str(), nullptr, 2);
            readToken();
            return static_cast<uint16_t>( i );
        }
        else if( current.type == HEXA_VALUE )
        {
            value = value.substr( 2, value.length() - 1 ); // remove the base before number eg : 0b0101 -> 0101
            if( value.length() > 4 )
            {
                compileError( "Value '" + current.text + "' is too big to be encoded.\n\trange: [0, 65535] or [-32768, 32767]" );
                return false;
            }
            long int i = std::stol( value.c_str(), nullptr, 16);
            readToken();
            return static_cast<uint16_t>( i );
        }
        compileError( "Expected a value" );
        return 0;
    }

    // parse char values    
    char Compiler::parseCharValue( void )
    {
        string s = current.text;
        string c = "";

        c += s[1];
        if( s.length() == 3 )
        {
            readToken();
            return  s[1] ;
        }
        if( s.length() == 4 )
        {
            c += s[2];
            if     ( c == "\\n" ) 
            {
                readToken();
                return '\n' ;
            }
            else if( c == "\\t" ) 
            {
                readToken();
                return '\t' ;
            }
            else if( c == "\\v" ) 
            {
                readToken();
                return '\v' ;
            }
            else if( c == "\\s" ) 
            {
                readToken();
                return ' ' ;
            }
            else if( c == "\\," )
            {
                readToken();
                return ',';
            }
            else if( c == "\\#" ) // avoid comment
            {
                readToken();
                return '#';
            }
            else if( c == "\\@" ) // avoid comment
            {
                readToken();
                return '@';
            }
            else if( c == "\\|" ) // avoid comment
            {
                readToken();
                return '|';
            }
            else 
            {
                readToken();
                return s[3];
            }
        }
        return '?' ; // unknown character
    }


    // tokenize a split line  ( called after lexer::splitLine )
    void Compiler::tokenizeOneLine( const string& line )  
    {
        vector<string> words = lexer::splitLine( line );
        for( uint32_t i=0; i<words.size(); i++)
        {
            token t = lexer::tokenizeOneWord( words[i] );
            tokens.push_back( t );
        }
    }

    // load a file and tokenize it // TODO Refactor with lexer removespace
    bool Compiler::loadAndTokenize( string fileName )
    {

        char line[120];    // char * used to store line
        std::ifstream rfile;    
        rfile.open( fileName );    // Open file
        if( rfile.is_open())
        {
            token first_token(";", ENDL);  // used for error message in case or error on line 1
            tokens.push_back( first_token );

            while(rfile.getline( line, 120 ))    // tokenize whole line for every lines
            {
                tokenizeOneLine( line );
            }
        }
        else
        {
            rfile.close();
            cerr << "/!\\ Error while compiling : Cannot open file '" << fileName << "'." << endl;
            exit(-1);
            return false;

        }
        rfile.close();
        token last_token("STOP", STOP); 
        tokens.push_back( last_token );
        return true;
    }

    // parse one instruction from the token array
    bool Compiler::parseOneInstr( void ) // TODO 
    {
        if( current.text == "" ) // skip empty tokens. (might happens when using tabs mixed )
        {
            readToken();
            return true;
        }
        else if( current.type == ENDL ) 
        {
            return true; // do nothing
        }
        else
            return false;
        return false;
    }

}


