#include <iterator>
#include <fstream>
#include <cmath>
#include <vector>
#include "Compiler.h"
#include "lexer.h"

 
namespace basal
{
    // compile basal file to basal assembly
    bool Compiler::compile( string file_name )
    {
        this->fileName = file_name;
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
    void Compiler::throwCompileError( string error_message )
    {
        // find token number in the line
        unsigned i = this->j;
        if( current.type == ENDL ) i--;
        while( tokens[ i ].type != ENDL ) i--;
        i = j - i; // get diff

        // get the actual line
        char line[120];    // char * used to store line
        std::ifstream rfile;    
        rfile.open( fileName );    // Open file
        if( rfile.is_open())
        {
            for( unsigned x=0; x<lineNbr; x++)
            {
                rfile.getline( line, 120 );    // tokenize whole line for every lines
            }
        }
        // find where is the current token
        vector<string> tkns = lexer::splitLine( line );
        unsigned cpt = 0;   // used to count real tokens
        unsigned a = 0;     // iterate
        string mess = "";   // will contains the source code of the line and an indicator of where the error is
        while( cpt != i )
        {
            if( cpt != 0 or not lexer::isSpace( tkns[a][0])) mess += tkns[a]; // reconstruct mess without first spaces
            if( not lexer::isSpace( tkns[a][0]) ) cpt++;  // dont count spaces as real tokens
            a++;
        }

        // reconstruct current line
        a = mess.length();
        // add error indicator under  ex :       var a = 134 < aasdasd
        //                                                        ^
        mess += "\n";
        for( unsigned x=a; a>0; a-- )
        {
            mess += " ";
        }
        // display a '^' just under the token which prompted the compile error
        unsigned mid = ( current.text.length() + current.text.length() % 2 ) / 2; // get word's middle char index
        for( unsigned x=current.text.length(); x>0; x-- )
        {
            if( x != mid ) mess += " ";
            else if( x == mid ) mess += "^";
        }

        if( frenchMessages )
            std::cout << "Erreur de compilation, ligne ";
        else if( not frenchMessages )
            std::cout << "Compile error, line "; 
        std::cout << lineNbr << "\n -> " << error_message << "\n    " << mess << std::endl;

        encounteredError = true;    // stop the compilation process
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

    // parse char values // TODO remove ?
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
        vector<string> words = lexer::splitLine( line, false );
        for( uint32_t i=0; i<words.size(); i++)
        {
            token t = lexer::tokenizeOneWord( words[i] );
            tokens.push_back( t );
        }
    }

    // load a file and tokenize it
    bool Compiler::loadAndTokenize( string file )
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
        else // File hasn't been opened
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

    using basal::Type;
    // Expression := SimpleExpression [ RelationalOperator SimpleExpression ]
    Type Compiler::parseExpression( void )
    {
        Type type = INT;

        parseSimpleExpression();

        if( current.type == RELOP )
        {
            type = BIN;
            program << "    pop ax" << endl;
            program << ":CMP_" << tagNumber << endl;

            string txt = current.text; // buffer

            readToken(); // read relationnal operator
            parseSimpleExpression();

            program << "    pop bx" << endl;
            program << "    cmp ax, bx" << endl;

            if( txt == "==" )
            {
                program << "    jump CMP_TRUE_" << tagNumber << " if EQU" << endl;
            }
            else if( txt == "!=" )
            {

            }
            else if( txt == ">" )
            {

            } 
            else if( txt == "<" )
            {

            }
            else if( txt == "<=" )
            {

            }
            else if( txt == ">=" )
            {

            }
            readToken(); // read relationnal operator
            parseSimpleExpression();
            add("   pop bx"); // second operand
        }
    
        return type;
    }

    // SimpleExpression := Term { additiveOperator Term }
    Type Compiler::parseSimpleExpression( void )
    {

        return INT;
    }

    // Term := Factor { additiveOperator Factor }
    Type Compiler::parseTerm( void )
    {

    }

    // Factor := number | identifier | "(" Expression ")"
    Type Compiler::parseFactor( void )
    {
         
    }


}


