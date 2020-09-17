#pragma once
#include "BasalDef.h"

#include <iostream>
#include <sstream>
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
using std::stringstream;


        
namespace basal   // keep things contained in a namespace.  basm = Basal Assembly
{

    class Compiler
    {
    public:
        string basm_program;                    // store all the instructions

    private:
        unsigned j{ 0 };                            // used to count tokens
        unsigned lineNbr{ 0 };                      // one empty line is always artifially added at the begining
        vector<token> tokens;                       // store all tokens
    public: // debugging purposes
        map<string, uint16_t> declared_identifiers; // store addresses of labels
    private:
        token current;                  // used as current token
        string fileName;                // store source file name
        stringstream program;                 // contains basm program
        bool frenchMessages{ 0 };       // switch error messages to french
        unsigned tagNumber{ 0 };        // used to differenciate labels generation

    public:
        // assemble instructions
        bool compile( string file_name );
    
    private:

        // better error message for compilation
        void throwCompileError( string error_message );

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
        bool loadAndTokenize( string );

        // look at a token and redirect toward the appropriatre function, eg : ADD -> call parseAddBasedInstr()     
        bool parseOneInstr( void );

        // Expr := SimpleExpr [ RelationalOperator SimpleExpr ]
        void parseExpression( void );

        // SimpleExpression := Term { additiveOperator Term }
        void parseSimpleExpression( void );

    };
}
