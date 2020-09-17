#pragma once
#include "BasalDef.h"

#include <iostream>
#include <string>
#include <vector>
#include <map>

using std::string;
using std::vector;
using std::map;
using std::cout;
using std::cerr;
using std::endl;

using lexer::token;


        
namespace basal   // keep things contained in a namespace.  basm = Basal Assembly
{

    class Compiler
    {
    public:
        string basm_program;                    // store all the instructions

    //private:
        uint64_t j{ 0 };                        // used to count tokens
        uint64_t lineNbr{ 0 };                  // one empty line is always artifially added at the begining
        vector<token> tokens;                   // store all tokens
        map<string, uint16_t> declared_identifiers;  // store addresses of labels
        token current;                          // used as current token

    public:
        // assemble instructions
        bool compile( string fileName );
    
    private:

        // better error message for compilation
        bool compileError( string message );

        // increment j and reassign token t
        bool readToken( void );

        // curent token must be a ENDL, compileError and return false otherwise
        bool readEndl( void );

        // current token must be a comma, compileError and return false otherwise
        bool readComma( void );

        // parse decimals, binary and hexadecimal values
        uint16_t parseValue( void ); 

        // parse characters
        char parseCharValue( void );

        // get one token from a string already split 
        void tokenizeOneLine( const string& line );

        // load a file and tokenize it
        bool loadAndTokenize( string fileName );

        // look at a token and redirect toward the appropriatre function, eg : ADD -> call parseAddBasedInstr()     
        bool parseOneInstr( void );

    };
}
