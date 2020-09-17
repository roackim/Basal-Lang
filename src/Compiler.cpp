#include <iterator>
#include <fstream>
#include <cmath>
#include <vector>
#include "Compiler.h"
#include "lexer.h"

 
namespace basal
{
    // default constructor of the compiler object
    Compiler::Compiler( void )
    {
        p_scope = new Scope( nullptr ); // Global Scope
    }

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

        current = tokens[j];

        while( dispatchFunctionCall() ){ readEndl(); }
        
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
        char line[120];             // char * used to store line
        std::ifstream rfile;    
        rfile.open( fileName );     // Open file
        if( rfile.is_open())
        {
            for( unsigned x=0; x<lineNbr; x++)
            {
                rfile.getline( line, 120 );  
            }
            rfile.close();
        }
        else // File hasn't been opened
        {
            rfile.close();
            string message = "Cannot open file '" + fileName + "'" ;
            if( frenchEnabled ) message = "Impossible d'ouvrir le fichier '" + fileName + "'" ;
            throwSimpleError( message );
        }
        // find where is the current token
        vector<string> tkns = lexer::splitLine( line, true );

        string source = "";
        string helper = "\n    ";

        unsigned a=0;
        while( lexer::isSpace( tkns[0][0] )) a++;
        // reconstruct current line
        while( not( tkns[a] == "#" or tkns[a] == ";") )
        {
            source += tkns[a]; // reconstruct mess without first spaces
            char c = ' ';
            if( i == 1 ) 
            {
                if( not lexer::isSpace( tkns[a][0] )) c = '^';
            }
            for( unsigned x=0; x<tkns[a].length(); x++ )
            { 
                helper += c; 
            }
            if( not lexer::isSpace( tkns[a][0]) ) 
            {   
                if( i != 0 ) i--;  // dont count spaces as real tokens
            }
            a++;
        }

        source += helper;

        cerr << endl;
        if( frenchEnabled )
            std::cerr << "Erreur de compilation, ligne ";
        else if( not frenchEnabled )
            std::cerr << "Compile error, line "; 

        std::cerr << lineNbr << "\n -> " << error_message << "." << "\n\n    " << source << std::endl;
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
    void Compiler::checkOperandTypes( string OP, Type type1, Type type2 )
    {
        string op = lexer::to_upper( OP );
        // Error message
        string message = "The operator '" + OP + "' is not compatible with operands types '" 
            + basal::getStringFromType( type1 ) + "' and '" + basal::getStringFromType( type2 ) + "'";
        if( frenchEnabled ) message = "L'opérateur '" + OP + "' est imcompatible avec les opérandes de types '" 
            + basal::getStringFromType( type1 ) + "' et '" + basal::getStringFromType( type2 ) + "'";

        if( op=="-" or op=="*" or op=="/" or op=="^" or op=="%" ) // var operands expected
        {
            if( type1 != VAR or type2 != VAR )
                throwCompileError( message );
        }
        else if( op=="OR" or op=="OU" or op=="AND" or op=="ET" or op=="." ) // bin operands expected
        {
            if( type1 != BOOL or type2 != BOOL )
                throwCompileError( message );
        }
        else if( op=="+" )
        {
            if( type1 != type2 )
            {
                throwCompileError( message );
            }
        }
    }

    // Call throwCompileError if incompatible type
    void Compiler::checkOperandTypes( string OP, Type type1 )
    {
        string op = lexer::to_upper( OP );
        // Error message
        string message = "The operator '" + OP + "' is not compatible with operand type '" 
            + basal::getStringFromType( type1 ) + "'";
        if( frenchEnabled ) message = "L'opérateur '" + OP + "' est imcompatible avec l'opérande de type '" 
            + basal::getStringFromType( type1 ) + "'";

        if( op=="NOT" or op=="NON" or op=="!" )
        {
            if( type1 != BOOL )
                throwCompileError( message );
        } 
    }

    // helper function to generate basm instructions
    string Compiler::getInstrFromADDorMUL( string op, Type type1, Type type2 )
    {
        tagNumber++;
        op = lexer::to_upper( op );
        if     ( op=="+" and type1 == VAR and type2 == VAR ) return "add ";
        else if( op=="-" ) return "sub "; 
        else if( op=="*" ) return "mul ";
        else if( op=="/" ) return "div ";
        else if( op=="%" ) return "mod ";
        else if( op=="AND" or op=="ET" or op=="." ) return "and ";
        else if( op=="OR"  or op=="OU" or op=="+" ) return "or  ";
        else if( op=="^" )
        {
            stringstream s;
            s << ":POW_" << tagNumber << endl;
            s << "    copy bx, cx"  << endl;
            s << "    copy ax, bx"  << endl;
            s << "    copy  1, ax"  << endl;
            s << ":POW_LOOP_" << tagNumber << endl;
            s << "    cmp  0, cx"   << endl;
            s << "    jump POW_END_" << tagNumber << " if EQU" << endl;
            s << "    mul  bx, ax"  << endl;
            s << "    sub  1, cx"   << endl;
            s << "    jump POW_LOOP_" << tagNumber << endl;
            s << ":POW_END_" << tagNumber << endl;
            return s.str();

        }
        else
            throwSimpleError("Should not happen: error in getInstrFromADDorMUL(), token: " + op);
        return "";
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
            if( current.text == "#" ) lineNbr++; // do not increment line Nbr if used ';' to separate 2 instructions
            readToken();
            return true;
        }
        else
        {
            string mess = "Expected End of line after instruction";
            if( frenchEnabled ) mess = "Retour a la ligne attendu apres une instruction";
            throwCompileError( mess );
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


    using basal::Type;
    // Expression := SimpleExpression [ RelationalOperator SimpleExpression ]
    Type Compiler::parseExpression( void )
    {
        Type type = parseSimpleExpression();

        if( current.type == RELOP )
        {
            type = BOOL;
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
            program << "    jump CMP_END_" << tagNumber << endl;
            program << ":CMP_TRUE_" << tagNumber << endl;
            program << "    push 1" << endl;
            program << ":CMP_END_" << tagNumber << endl;
        }
    
        return type;
    }

    // SimpleExpression := Term { additiveOperator Term }
    Type Compiler::parseSimpleExpression( void )
    {
        Type type1 = parseTerm();

        while( current.type == ADDOP )
        {
            string op = lexer::to_upper( current.text );
            readToken();
            
            Type type2 = parseTerm();

            string instr = getInstrFromADDorMUL( op, type1, type2 );
            program << "    pop  bx" << endl;   // right operand
            program << "    pop  ax" << endl; 
            program << "    " << instr << " bx, ax" << endl;
            program << "    push ax" << endl;

            checkOperandTypes( op, type1, type2 ); // doesn't account for relationnal operators
            type1 = type2;
        }
        
        return type1;
    }

    // Term := Factor { multiplicativeOperator Factor }
    Type Compiler::parseTerm( void )
    {
        Type type1 = parseFactor();
        while( current.type == MULOP )
        {
            string op = lexer::to_upper( current.text );
            readToken();
            
            Type type2 = parseFactor();

            string instr = getInstrFromADDorMUL( op, type1, type2 );
            program << "    pop  bx" << endl;   // right operand
            program << "    pop  ax" << endl;   // left operand
            if( op=="^" )
                program << instr << endl;
            else
                program << "    " << instr << " bx, ax" << endl;
            program << "    push ax" << endl;
            
            checkOperandTypes( op, type1, type2 ); // doesn't account for relationnal operators
            type1 = type2;
        }
        
        return type1;
    }

    // Factor := number | identifier | "(" Expression ")"
    Type Compiler::parseFactor( void )
    {

        if( current.type == DECIMAL_VALUE or current.type == HEXA_VALUE or current.type == BINARY_VALUE or current.text == "-")
        {
            uint16_t value = parseValue();
            program << "    push " << value << endl;
            return VAR;

        } 
        else if( current.type == RESERVED_VALUE )
        {
            string s = lexer::to_upper( current.text );
            if( s == "TRUE" or s == "VRAI" )
                program << "    push 1" << endl;
            else if( s == "FALSE" or s == "FAUX" )
                program << "    push 0" << endl;

            readToken();
            return BOOL;
        }
        else if( current.type == NOT )
        {
            string op = current.text;
            readToken(); // read 'not' operator

            Type type = parseFactor();
            
            program << "    pop  ax" << endl;
            program << "    add  1, ax" << endl;
            program << "    mod  2, ax" << endl;
            program << "    push ax" << endl;
            checkOperandTypes( op, type );

            return type;

        }
        else if( current.type == IDENTIFIER ) // not a declaration, identifier is used as a variable
        {
            string varName = current.text;
            Variable var = p_scope->getVar( varName );
            // throw error if var is undelared
            if( var.type == UNDECLARED )
            {
                string mess = "The variable '" + varName + "' doesn't exist";
                if( frenchEnabled ) mess = "La variable '" + varName + "' n'existe pas";
                throwCompileError( mess );
            }
            // var is declared
            program << "# using var: " << varName << endl;
            program << "    copy fx, ax" << endl;
            program << "    add  " << var.offset << ", ax" << endl;
            program << "    copy (ax), bx" << endl;
            program << "    push bx" << endl;

            readToken();
            return var.type;

        }
        else if( current.type == LPAREN )
        {
            readToken(); // read left parent
            Type type = parseExpression();
            if( current.type == RPAREN ) readToken();
            else  // Error
            {
                string message = "Missing closing parenthesis";
                if( frenchEnabled ) message = "Parenthèse fermante manquante";
                throwCompileError( message );
            }
            return type;
        }
        else    // throw error
        {
            string message = "Unexpected token: '" + current.text + "'|" ;
            if( frenchEnabled ) message = "Symbole innatendu '" + current.text + "'|" ;
            throwCompileError( message );
        }
        return UNDECLARED;
    }

    // VarDeclaration := type identifier [ "=" Expression ]
    void Compiler::parseVarDeclaration( void )
    {
        if( current.type != TYPE ) // expect a type 
        {
            string mess = "Expected a type";
            if( frenchEnabled ) mess = "Type attendu";
            throwCompileError( mess );
        }

        Type varType = basal::getTypeFromString( current.text );
        readToken(); // read type

        if( current.type != IDENTIFIER ) // expect an identifier
        {
            string mess = "Indentifier expected";
            if( frenchEnabled ) mess = "Identifiant attendu";
            throwCompileError( mess );
        }
        // check if the variable has already been declared
        string varName = current.text;
        Variable var = p_scope->getVar( varName );

        if( var.type != UNDECLARED )
        {
            string mess = "The variable '" + varName + "' already exists";
            if( frenchEnabled ) mess = "La variable '" + varName + "' existe déjà";
            throwCompileError( mess );
        }

        p_scope->declareVar( varName, varType );

        program << "# declaring var: " << varName << endl;
        program << "    push 0 " << endl; // create variable

        if( tokens[j+1].text == "=" ) parseAssignement();
        else readToken(); // read identifier

    }

    // Assignement := Identifier "=" Expression
    void Compiler::parseAssignement( void )
    {
        if( current.type != IDENTIFIER ) // expect a type 
        {
            string mess = "Expected an identifier";
            if( frenchEnabled ) mess = "Identifiant attendu";
            throwCompileError( mess );
        }
        // we can assume current.type is an identifier

        string varName = current.text;
        Variable var = p_scope->getVar( varName );

        if( var.type == UNDECLARED )
        {
            string mess = "The variable '" + varName + "' doesn't exist";
            if( frenchEnabled ) mess = "La variable '" + varName + "' n'existe pas";
            throwCompileError( mess );
        }
        // we can assume the varaible has been declared

        readToken();
        if( current.text == "=" ) readToken();
        
        Type exprType = parseExpression();

        // [ code gen ]

        program << "# assignement to var: " << varName << endl;
        program << "    copy fx, ax" << endl;
        program << "    add  " << var.offset << ", ax" << endl;
        program << "    pop  bx" << endl;
        program << "    copy bx, (ax)" << endl << endl;

    }

    // redirect to the corresponding function depending on the first token
    bool Compiler::dispatchFunctionCall( void )
    {

        if( current.type == TYPE ) parseVarDeclaration();
        else if( current.type == IDENTIFIER ) parseAssignement();
        else if( current.type == STOP ) return false;
        else if( current.type == ENDL ) return true;
        else
        {
            string mess = "Unrecognized instruction";
            if( frenchEnabled ) mess = "Instruction inconnue";
            throwCompileError( mess );
        }
        return true;
    }

}


