#include <iterator>
#include <fstream>
#include <cmath>
#include <vector>
#include "Compiler.h"
#include "lexer.h"

 
namespace basal
{
    // compile basal file to basal assembly
    void Compiler::compile( string file_name )
    {
        this->fileName = file_name;
        try
        {
            loadAndTokenize( fileName ); // error while tokenizing
        } catch( const std::exception& e )
        {
            return; // stop compilation
        }


        if( !tokens.empty() ) //  TODO change
        {
            current = tokens[j];
            while( current.type == ENDL ) readToken();

            parseExpression(); // parse an expression
        }
    }

    // throw and output a comprehensible error message, throw a runtime_error
    void Compiler::throwCompileError( string error_message )
    {
        // find token number in the line
        unsigned i = j;
        if( current.type == ENDL and j != 0 ) i--;
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

        if( frenchEnabled )
            std::cerr << "Erreur de compilation, ligne ";
        else if( not frenchEnabled )
            std::cerr << "Compile error, line "; 

        std::cerr << lineNbr << "\n -> " << error_message << "\n    " << mess << std::endl;
        throw std::runtime_error( error_message );
    }

    // throw and output a simple error, without source code, useful for error outside compilation 
    // example : source file not found for example
    void Compiler::throwSimpleError( string error_message )
    {
        if( frenchEnabled )
            std::cerr << "Erreur lors de la compilation:";
        else if( not frenchEnabled )
            std::cerr << "Error encountered during compilation:"; 
        std::cerr << "\n -> " << error_message << endl;
        throw std::runtime_error( error_message );
    }


    // Call throwCompileError if incompatible types 
    void Compiler::checkOperandTypes( Type type1, string OP, Type type2 )
    {
        string op = lexer::to_upper( OP );
        if( op=="+" or op=="-" or op=="*" or op=="/" or op=="^" or op=="%" ) // var operands expected
        {

        }
        else if( op=="OR" or op=="OU" or op=="AND" or op=="ET" ) // bin operands expected
        {
            if( type1 != BIN or type2 != BIN )
            {
                string message = "";
                if( frenchEnabled ) message = "Types incompatibles avec l'operateur '" + OP + "'";
                else if( not frenchEnabled ) message = "Incompatible types with operator '" + OP + "'";
                throwCompileError( message );
            }
        }
    }

    // helper function to generate basm instructions
    string Compiler::getInstrFromADDorMUL( string op )
    {
        op = lexer::to_upper( op );
        if     ( op=="+" ) return "add ";
        else if( op=="-" ) return "sub "; 
        else if( op=="*" ) return "mul ";
        else if( op=="/" ) return "div ";
        else if( op=="^" ) return "pow ";
        else if( op=="%" ) return "mod ";
        else if( op=="AND" or op=="ET" ) return "and ";
        else if( op=="OR"  or op=="OU" ) return "or  ";
        else
            throwSimpleError("Should no happen: error in getInstrFromADDorMUL()");
    }

    // increment j and reassign token t
    bool Compiler::readToken( void )
    {
        if( j < tokens.size() - 2 )
        {    
            current = tokens[ ++j ]; // go to next token
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
            throwCompileError("Expected End of line after instruction");
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
            throwCompileError("Expected a comma, not '" + current.text + "'");
            return false;
        }
    }

    // parse decimals, binary and hexadecimal value, return a uint16 encoded value, and call readToken()
    uint16_t Compiler::parseValue( void )
    {

        int sign = 1;
        if( current.text == "-" )
        {
            sign = -1;
            readToken();
        }
        string value = current.text;
        if( current.type == DECIMAL_VALUE )
        {
            int32_t i = atoi( value.c_str() );
            if( i > 65535 or i < -32768 )
            {
                throwCompileError( "Value '" + current.text + "' is too big to be encoded.\n\trange: [0, 65535] or [-32768, 32767]" );
            }
            readToken();
            return static_cast<uint16_t>( sign*i );
        }
        else if( current.type == BINARY_VALUE )
        {
            value = value.substr( 2, value.length() - 1 ); // remove the base before number eg : 0b0101 -> 0101
            if( value.length() > 16 )
            {
                throwCompileError( "Value '" + current.text + "' is too big to be encoded.\n\trange: [0, 65535] or [-32768, 32767]" );
            }
            long int i = std::stol( value.c_str(), nullptr, 2);
            readToken();
            return static_cast<uint16_t>( sign*i );
        }
        else if( current.type == HEXA_VALUE )
        {
            value = value.substr( 2, value.length() - 1 ); // remove the base before number eg : 0b0101 -> 0101
            if( value.length() > 4 )
            {
                throwCompileError( "Value '" + current.text + "' is too big to be encoded.\n\trange: [0, 65535] or [-32768, 32767]" );
            }
            long int i = std::stol( value.c_str(), nullptr, 16);
            readToken();
            return static_cast<uint16_t>( sign*i );
        }
        throwCompileError( "Expected a value" );
        return 0;
    }

    // tokenize a split line  ( called after lexer::splitLine )
    void Compiler::tokenizeOneLine( const string& line )  
    {
        vector<string> words = lexer::splitLine( line, false );
        for( uint32_t i=0; i<words.size(); i++)
        {
            token t = lexer::tokenizeOneWord( words[i] );
            tokens.push_back( t );
            // cout << tokens.size() << " -> " << t.text << endl;
        }
    }

    // load a file and tokenize it
    void Compiler::loadAndTokenize( string file )
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
            string message = "Cannot open file '" + fileName + "'" ;
            if( frenchEnabled ) message = "Impossible d'ouvrir le fichier '" + fileName + "'" ;
            throwSimpleError( message );

        }
        rfile.close();
        token last_token("STOP", STOP); 
        tokens.push_back( last_token );
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
            program << "    pop  ax" << endl;
            program << ":CMP_" << tagNumber << endl;

            string txt = current.text; // buffer current operator 
 
            readToken(); // read relationnal operator
            parseSimpleExpression();

            tagNumber++;

            program << "    pop  bx" << endl;
            program << "    cmp  ax, bx" << endl;

            if( txt == "==" )
            {
                program << "    jump CMP_TRUE_" << tagNumber << " if EQU" << endl;
            }
            else if( txt == "!=" )
            {
                program << "    jump CMP_TRUE_" << tagNumber << " ifnot EQU" << endl;
            }
            else if( txt == ">" )
            {
                program << "    jump CMP_TRUE_" << tagNumber << " if NEG" << endl;
            } 
            else if( txt == "<" )
            {
                program << "    jump CMP_TRUE_" << tagNumber << " if POS" << endl;
            }
            else if( txt == "<=" )
            {
                program << "    jump CMP_TRUE_" << tagNumber << " if EQU" << endl;
                program << "    jump CMP_TRUE_" << tagNumber << " if POS" << endl;
            }
            else if( txt == ">=" )
            {
                program << "    jump CMP_TRUE_" << tagNumber << " if EQU" << endl;
                program << "    jump CMP_TRUE_" << tagNumber << " if NEG" << endl;
            }
            program << ":CMP_FALSE_" << tagNumber << endl;
            program << "    push 0" << endl;
            program << "    jump CPM_END_" << tagNumber << endl;
            program << ":CMP_TRUE_" << tagNumber << endl;
            program << "    push 1" << endl;
            program << ":CMP_END_" << tagNumber << endl;
        }
    
        return type;
    }

    // SimpleExpression := Term { additiveOperator Term }
    Type Compiler::parseSimpleExpression( void )
    {
        Type type = parseTerm();

        while( current.type == ADDOP )
        {
            string op = lexer::to_upper( current.text );
            readToken();
            
            Type type2 = parseTerm();

            string instr = getInstrFromADDorMUL( op );
            program << "    pop  ax" << endl;   // right operand
            program << "    pop  bx" << endl; 
            program << "    " << instr << " ax, bx" << endl;
            program << "    push bx" << endl;

            checkOperandTypes( type, op, type2 ); // doesn't account for relationnal operators
            type = type2;
        }
        
        return type;
    }

    // Term := Factor { multiplicativeOperator Factor }
    Type Compiler::parseTerm( void )
    {
        Type type = parseFactor();
        while( current.type == MULOP )
        {
            string op = lexer::to_upper( current.text );
            readToken();
            
            Type type2 = parseFactor();

            string instr = getInstrFromADDorMUL( op );
            program << "    pop  ax" << endl;   // right operand
            program << "    pop  bx" << endl; 
            program << "    " << instr << " ax, bx" << endl;
            program << "    push bx" << endl;
            
            checkOperandTypes( type, op, type2 ); // doesn't account for relationnal operators
            type = type2;
        }
        
        return type;
    }

    // Factor := number | identifier | "(" Expression ")"
    Type Compiler::parseFactor( void )
    {
        Type type;
        if( current.type == DECIMAL_VALUE or current.type == HEXA_VALUE or current.type == BINARY_VALUE or current.text == "-")
        {
            uint16_t value = parseValue();
            program << "    push " << value << endl;
            type = INT;

        } 
        else if( current.type == IDENTIFIER ) // not a declaration, identifier is used as a variable
        {
            throwCompileError("Identifiers are not implemented yet");

            // TODO variables & functions
            if( false ) // TODO SCOPE var declaration
            {
                string message;
                if( tokens[j+1].type == LPAREN ) // identifier is a function name
                {
                    message = "The function '" + current.text + "' does not exists"; 
                    if( frenchEnabled ) message = "La fonction '" + current.text + "' n'existe pas";
                }
                else // identifier is a variable
                {
                    message = "The variable '" + current.text + "' does not exists";
                    if( frenchEnabled ) message = "La variable '" + current.text + "' n'existe pas";
                    if( false )// variableExistInAnotherScope() // TODO // more precise error message
                    {
                        if( frenchEnabled ) message += " dans ce champ";
                        else message += " in this scope";
                    }
                }
                throwCompileError( message ); // throw an error, terminate compilation
            }
            else // function exists or variable exists in the current scope 
            {
                ;
            }
            // TODO get variable type
            type = INT; // TODO subject to change depending on the variable type

        }
        else if( current.type == LPAREN )
        {
            readToken(); // read left parent
            type = parseExpression();
            if( current.type == RPAREN ) readToken();
            else  // Error
            {
                string message = "Missing closing parenthesis";
                if( frenchEnabled ) message = "Parenthèse fermante manquante";
                throwCompileError( message );
            }
        }
        else    // throw error
        {
            string message = "Unexpected token: '" + current.text + "'|" + getTokenTypeStr(current.type);
            if( frenchEnabled ) message = "Symbole innatendu '" + current.text + "'|" + getTokenTypeStr(current.type);
            throwCompileError( message );
        }
        return type;
    }


}


