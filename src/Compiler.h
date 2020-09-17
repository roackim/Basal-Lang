#pragma once
#include "BasalDef.h"
#include "Scope.h"

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
        Compiler( void );                       // default constructor
        string basm_program;                    // store all the instructions
        // TODO put private below, only public for debugging purposes
        vector<token> tokens;                   // store all tokens
        stringstream program;                   // contains basm program
        unsigned j = 0;                         // used to count tokens
        bool frenchEnabled = false;         // switch error messages to french
    private:

        Scope* p_scope = nullptr;           // current scope
        unsigned lineNbr = 1;               // one empty line is always artifially added at the begining
        map<string, uint16_t> variables;    // store addresses of labels
        unsigned stackPointer = 0;
        token current;                      // used as current token
        string fileName;                    // store source file name


        unsigned tagNumber = 0;             // used to differenciate labels generation

    public:
        // compile to basal assembly
        void compile( string file_name );
    
    private:

        // better error message for compilation
        void throwCompileError( string error_message );
        // throw and output a simple error, without source code, useful for error outside compilation 
        // example : source file not found for example
        void throwSimpleError( string error_message );

        // Call throwCompileError if incompatible types 
        void checkOperandTypes( string OP, Type type1, Type type2 );

        // Call throwCompileError if incompatible type
        void checkOperandTypes( string OP, Type type1 );

        // helper function to generate basm instructions
        string getInstrFromADDorMUL( string op, Type type1, Type type2 );

        // increment j and reassign token t
        bool readToken( void );

        // curent token must be a ENDL, compileError and return false otherwise
        bool readEndl( void );

        // current token must be a comma, compileError and return false otherwise
        bool readComma( void );

        // parse decimals, binary and hexadecimal values
        uint16_t parseValue( void ); 

        // get one token from a string already split 
        void tokenizeOneLine( const string& line );

        // load a file and tokenize it
        void loadAndTokenize( string );

        // Expr := SimpleExpr [ RelationalOperator SimpleExpr ]
        Type parseExpression( void );

        // SimpleExpression := Term { additiveOperator Term }
        Type parseSimpleExpression( void );

        // Term := Factor { additiveOperator Factor }
        Type parseTerm( void );

        // Factor := number | identifier | "(" Expression ")"
        Type parseFactor( void );

        // VarDeclaration := type identifier [ "=" Expression ]
        void parseVarDeclaration( void );

        // Assignement := Identifier "=" Expression
        void parseAssignement( void );

        bool dispatchFunctionCall( void );

    };
}
