#include <iterator>
#include <fstream>
#include <cmath>
#include <vector>

#include "Compiler.h"
#include "lexer.h"

// save diagnostic state
#pragma GCC diagnostic push 

#include "half.hpp"

// turn the warnings back on
#pragma GCC diagnostic pop
 
using namespace half_float;

namespace basal
{
    // default constructor of the compiler object
    Compiler::Compiler( void )
    {
        currentScope = new Scope( nullptr );
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

        while( parseStatement() ){ readEndl(); } // require ENDL after each instructions
        
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
        while( lexer::isSpace( tkns[a][0] )) a++;

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
    Type Compiler::checkOperandTypes( string OP, Type type1, Type type2 )
    {
        string op = lexer::to_upper( OP );
        // Error message
        string message = "The operator '" + OP + "' is not compatible with operands types '" 
            + getStringFromType( type1 ) + "' and '" + getStringFromType( type2 ) + "'";
        if( frenchEnabled ) message = "L'opérateur '" + OP + "' est imcompatible avec les opérandes de types '" 
            + getStringFromType( type1 ) + "' et '" + getStringFromType( type2 ) + "'";

        if(( type1 == FLOAT and type2 == INT ) or ( type1 == INT and type2 == FLOAT ) or ( type1 == FLOAT and type2 == FLOAT ))
        {
            if( op=="+" or op=="-" or op=="*" or op=="/" )
                return FLOAT;
        }
        if( type1 == FLOAT and type2 == INT )
        {
            if( op == "^" )
                return FLOAT;
        }
        if( op=="-" or op=="*" or op=="/" or op=="^" or op=="%" ) // var operands expected
        {
            if( type1 != INT or type2 != INT )
                throwCompileError( message );
            return INT;
        }
        else if( op=="OR" or op=="OU" or op=="AND" or op=="ET" or op=="." ) // bin operands expected
        {
            if( type1 != BIN or type2 != BIN )
                throwCompileError( message );
            return BIN;
        }
        else if( op=="+" or op=="=")
        {
            if( type1 == FLOAT and type2 == INT )
            {
                return FLOAT;
            }
            else if( type1 != type2 )
            {
                throwCompileError( message );
            }
            return type1;
        }
        else if( lexer::matchRELOP( op ) )
        {
            if( type1 != type2 )
            {
                throwCompileError( message );
            }
            return BIN;
        }
        else
            throwCompileError( message );
        return UNDECLARED;
    }

    // Call throwCompileError if incompatible type
    void Compiler::checkOperandTypes( string OP, Type type1 )
    {
        string op = lexer::to_upper( OP );
        // Error message
        string message = "The operator '" + OP + "' is not compatible with operand type '" 
            + getStringFromType( type1 ) + "'";
        if( frenchEnabled ) message = "L'opérateur '" + OP + "' est imcompatible avec l'opérande de type '" 
            + getStringFromType( type1 ) + "'";

        if( op=="NOT" or op=="NON" or op=="!" )
        {
            if( type1 != BIN )
                throwCompileError( message );
        } 
    }

    // helper function to generate basm instructions
    string Compiler::getInstrFromADDorMUL( string op, Type type1, Type type2 )
    {
        tagNumber++;
        op = lexer::to_upper( op );

        stringstream s;

        if( type1 == BIN and type2 == BIN )
        {
            if( op=="AND" or op=="ET" or op=="." ) s << "    and bx, ax";
            if( op=="OR"  or op=="OU" or op=="+" ) s << "    or  bx, ax";
        }
        else if( type1 == INT  and type2 == INT )
        {
            if     ( op=="+" ) s << "    add bx, ax" << endl; 
            else if( op=="-" ) s << "    sub bx, ax" << endl; 
            else if( op=="*" ) s << "    mul bx, ax" << endl;
            else if( op=="/" ) s << "    div bx, ax" << endl;
            else if( op=="%" ) s << "    mod bx, ax" << endl;
            else if( op=="^" )
            {
                s << ":POW_" << tagNumber << endl;
                s << "    copy bx, cx"  << endl;
                s << "    copy ax, bx"  << endl;
                s << "    copy  1, ax"  << endl;
                s << ":POW_LOOP_" << tagNumber << endl;
                s << "    cmp  0, cx"    << endl;
                s << "    jump POW_END_" << tagNumber << " if EQU" << endl;
                s << "    mul  bx, ax"   << endl;
                s << "    sub  1, cx"    << endl;
                s << "    jump POW_LOOP_" << tagNumber << endl;
                s << ":POW_END_" << tagNumber << endl;
            }
        }
        else if( type1 == FLOAT )
        {
            if( type2 == INT and op!="^" ) // case '^' type2 needs to be int
                s << "    itof bx" << endl;
            if( type2 == FLOAT or type2 == INT )
            {
                if     ( op=="+" ) s << "    fadd bx, ax" << endl; 
                else if( op=="-" ) s << "    fsub bx, ax" << endl; 
                else if( op=="*" ) s << "    fmul bx, ax" << endl;
                else if( op=="/" ) s << "    fdiv bx, ax" << endl;
                else if( op=="^" and type2==INT )
                {
                    s << ":POW_" << tagNumber << endl;
                    s << "    copy bx, cx"  << endl;
                    s << "    copy ax, bx"  << endl;
                    s << "    copy  1, ax"  << endl;
                    s << "    itof ax"      << endl;
                    s << ":POW_LOOP_" << tagNumber << endl;
                    s << "    cmp  0, cx"    << endl;
                    s << "    jump POW_END_" << tagNumber << " if EQU" << endl;
                    s << "    fmul  bx, ax"  << endl;
                    s << "    sub  1, cx"    << endl;
                    s << "    jump POW_LOOP_" << tagNumber << endl;
                    s << ":POW_END_" << tagNumber << endl;

                }
            }
        }
        else
            throwCompileError("Error in: getInstr");

        // returns the instructions needed
        return s.str();
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

    // current token must be a ENDL, compileError and return false otherwise
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

    // current token must be a colon, compileError and return false otherwise
    bool Compiler::readColon( void )
    {
        if( current.type == COLON )
        {
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
            prev_value = static_cast<uint16_t>( sign*i );
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
            prev_value = static_cast<uint16_t>( sign*i );
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
            prev_value = static_cast<uint16_t>( sign*i );
            return static_cast<uint16_t>( sign*i );
        }
        throwCompileError( "Expected a value" );
        return 0;
    }

    // parse Float Values
    half Compiler::parseFloatValue( void )
    {
        int sign = 1;
        if( current.text == "-" )
        {
            sign = -1;
            readToken();
        }

        string value = current.text;
        float i = atof( value.c_str() );
        cout << i << endl;
        if( i > 65500 or i < -65500 )
        {
            throwCompileError( "Value '" + current.text + "' is too big to be encoded.\n\trange: [0, 65535] or [-32768, 32767]" );
        }
        readToken();

        half h( i*sign );
        return h;
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
        Type type1 = parseSimpleExpression();

        if( current.type == RELOP )
        {
            program << ":CMP_" << tagNumber << endl;

            string op = current.text; // buffer current operator 
 
            readToken(); // read relationnal operator
            Type type2 = parseSimpleExpression();

            tagNumber++;
            
            checkOperandTypes( op, type1, type2 );


            program << "    pop  bx" << endl;
            program << "    pop  ax" << endl;
            if( type1 == FLOAT and type2 == FLOAT )
                program << "    fcmp ax, bx" << endl;
            else
                program << "    cmp  ax, bx" << endl;

            if( op == "==" )
            {
                program << "    jump CMP_TRUE_" << tagNumber << " if EQU" << endl;
            }
            else if( op == "!=" )
            {
                program << "    jump CMP_TRUE_" << tagNumber << " ifnot EQU" << endl;
            }
            else if( op == ">" )
            {
                program << "    jump CMP_TRUE_" << tagNumber << " if NEG" << endl;
            } 
            else if( op == "<" )
            {
                program << "    jump CMP_TRUE_" << tagNumber << " if POS" << endl;
            }
            else if( op == "<=" )
            {
                program << "    jump CMP_TRUE_" << tagNumber << " if EQU" << endl;
                program << "    jump CMP_TRUE_" << tagNumber << " if POS" << endl;
            }
            else if( op == ">=" )
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

            type1 = BIN;
        }
    
        return type1;
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
            program << instr ; 
            program << "    push ax" << endl;

            type1 = checkOperandTypes( op, type1, type2 ); // doesn't account for relationnal operators
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
            program << instr;
            program << "    push ax" << endl;
            
            type1 = checkOperandTypes( op, type1, type2 ); // doesn't account for relationnal operators
        }

        
        return type1;
    }

    // Factor := number | identifier | "(" Expression ")" | Factor '^' Factor
    Type Compiler::parseFactor( void )
    {

        Type type1 = UNDECLARED;

        if( current.type == FLOAT_VALUE or ( current.text == "-" and tokens[j+1].type == FLOAT_VALUE))
        {
            half h = parseFloatValue();
            uint16_t* p;
            p = (uint16_t*) &h;

            program << "    push " << *p << "              # push float " << h << endl;
            type1 = FLOAT; 
        }
        else if( current.type == DECIMAL_VALUE or current.type == HEXA_VALUE or current.type == BINARY_VALUE or current.text == "-")
        {
            uint16_t value = parseValue();
            program << "    push " << value << endl;
            type1 = INT;

        } 
        else if( current.type == RESERVED_VALUE )
        {
            string s = lexer::to_upper( current.text );
            if( s == "TRUE" or s == "VRAI" )
                program << "    push 1" << endl;
            else if( s == "FALSE" or s == "FAUX" )
                program << "    push 0" << endl;

            readToken();
            type1 = BIN;
        }
        else if( current.type == NOT )
        {
            string op = current.text;
            readToken(); // read 'not' operator

            type1 = parseFactor();
            
            program << "    pop  ax" << endl;
            program << "    add  1, ax" << endl;
            program << "    mod  2, ax" << endl;
            program << "    push ax" << endl;
            checkOperandTypes( op, type1 );

        }
        else if( current.type == IDENTIFIER ) // not a declaration, identifier is used as a variable
        {
            string var_name = current.text;
            Variable var = currentScope->getVar( current.text );
            prev_used_var = var.name;

            // throw error if var is undelared
            if( var.type == UNDECLARED )
            {
                string mess = "The variable '" + var_name + "' doesn't exist in this scope";
                if( frenchEnabled ) mess = "La variable '" + var_name + "' n'existe pas dans ce champ";
                throwCompileError( mess );
            }

            // var is declared
            program << endl;
            program << "# using var: " << var.name << endl;
            program << "    copy fx, ax" << endl;
            unsigned d = var.depth;
            while( d != 0 )
            {
                program << "    copy (ax), ax" << endl;
                d--;
            }
            program << "    add  " << var.offset << ", ax" << endl;
            program << "    copy (ax), ax" << endl;
            program << "    push ax" << endl;
            program << "# end of using var: " << var.name << endl;

            readToken();
            type1 = var.type;

        }
        else if( current.type == LPAREN )
        {
            readToken(); // read left parent
            type1 = parseExpression();
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
            string message = "Unexpected token: '" + current.text + "'" ;
            if( frenchEnabled ) message = "Symbole innatendu '" + current.text + "'" ;
            throwCompileError( message );
        }
        while( current.type == EXPOP )
        {
            string op = lexer::to_upper( current.text );
            readToken(); // read op
            
            Type type2 = parseFactor(); // right operand

            string instr = getInstrFromADDorMUL( op, type1, type2 );
            program << "    pop  bx" << endl;   // right operand
            program << "    pop  ax" << endl;   // left operand
            program << instr;                   // result stored in ax
            program << "    push ax" << endl;
            
            type1 = checkOperandTypes( op, type1, type2 ); // doesn't account for relationnal operators
        }

        return type1;
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
        string var_name = current.text;
        Variable var = currentScope->getVar( current.text );

        if( var.type != UNDECLARED )
        {
            string mess = "The variable '" + var_name + "' already exists";
            if( frenchEnabled ) mess = "La variable '" + var_name + "' existe déjà";
            throwCompileError( mess );
        }

        // might fail, hence the var_name buffer
        prev_assigned_var = var_name;                // store the var name as the last known assigned variable

        currentScope->declareVar( var_name, varType );

        program << "    push 0                  # declaring var: " << var_name << endl;

        if( tokens[j+1].type == EQU ) parseAssignement();
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

        string var_name = current.text;
        Variable var = currentScope->getVar( current.text );

        if( var.type == UNDECLARED )
        {
            string mess = "The variable '" + var_name + "' doesn't exist";
            if( frenchEnabled ) mess = "La variable '" + var_name + "' n'existe pas";
            throwCompileError( mess );
        }

        prev_assigned_var = var_name;                // store the var name as the last known assigned variable
        // we can assume the varaible has been declared
        readToken(); // read identifier

        unsigned old = j;
        if( current.type == EQU ) readToken(); // read '='
        else
        {
            string mess = "Symbole '=' expected";
            if( frenchEnabled ) mess = "Symbole '=' attendu";
            throwCompileError( mess );
        }

        
        Type exprType = parseExpression();

        uint16_t rvalue = prev_value;
        
        if( var.type == BIN and exprType == INT )
        {
            tagNumber++;
            if( rvalue == 0 ) 
                program << "    copy 0, (sp)" << endl;
            else if( rvalue == 1 )              
                program << "    copy 1, (sp)" << endl;
            else
            {
                j--;
                string mess = "Cannot assign this value to a variable of type 'bin'";
                if( frenchEnabled ) mess = "Impossible d'assigner cette valeur a une variable de type 'bin'";
                throwCompileError( mess );
            }

        } 
        else
        {
            unsigned curr = j;
            j = old;
            checkOperandTypes( "=", var.type, exprType );
            j = curr;
        }

        program << endl;
        accessVar( var );
        program << "    pop  bx" << endl;
        if( var.type == FLOAT and exprType == INT )
            program << "    itof bx" << endl;
        program << "    copy bx, (ax)" << endl;
        program << "# end of assignement to var: " << var.name << endl;


    }

    // IfStatement := "IF" <bool>Expression "THEN" ENDL {Statement} {"ELSE" "IF" <bool>Expression ':' ENDL  {Statement} } ["ELSE" "THEN" ENDL {Statement} ] "END"
    void Compiler::parseIfStatement( void )
    {
        readToken(); // read IF keyword

        unsigned tag = ++tagNumber;
        unsigned subIFS = 0;

        program << "# If Statement" << endl;
        program << ":IF_" << tag << "_" << ++subIFS << endl;

        Type exprType = parseExpression();
        if( exprType != BIN )
        {
            string mess = "Cannot use an expression of type '" + getStringFromType( exprType ) + "' as a condition" ;
            if( frenchEnabled ) mess = "Impossible d'utiliser une expression de type '" + getStringFromType( exprType ) + "' en tant que condition" ;
            throwCompileError( mess );
        }

        readColon();    
        readEndl();

        program << "    pop ax" << endl;
        program << "    cmp 0, ax" << endl;
        program << "    jump IF_" << tag << "_" << subIFS << "_END if EQU" << endl; // skip body if false

        createScope();

        string word = lexer::to_upper( current.text );
        while( word != "END" and word != "FIN" and word != "ELSE" and word != "SINON" and current.type != STOP )
        {
            parseStatement();
            readEndl();

            word = lexer::to_upper( current.text );
        }

        exitScope();

        program << "    jump IF_" << tag << "_END" << endl; // body has been executed
        program << ":IF_" << tag << "_" << subIFS << "_END" << endl;

        word = lexer::to_upper( current.text );
        while( ( word == "ELSE" or word == "SINON" ) and current.type != STOP ) // enter sub if ( ELSE IF )
        {
            readToken();

            word = lexer::to_upper( current.text );
            if( word == "IF" or word == "SI" ) // Else If 
            {
                readToken();
                program << ":IF_" << tag << "_" << ++subIFS << endl;

                Type SubIfExprType = parseExpression();
                if( SubIfExprType != BIN )
                {
                    string mess = "Cannot use an expression of type '" + getStringFromType( exprType ) + "' as a condition" ;
                    if( frenchEnabled ) mess = "Impossible d'utiliser une expression de type '" + getStringFromType( exprType ) + "' en tant que condition" ;
                    throwCompileError( mess );
                }
                word = lexer::to_upper(current.text);
                if( current.type != COLON )
                {
                    string mess = "Colon character ':' expected after a condition" ;
                    if( frenchEnabled ) mess = "Charactère ':' attendu après une condition" ;
                    throwCompileError( mess ); 
                }

                readToken(); // read ':'
                readEndl();     // read end of line

                program << "# ELSE IF" << endl;
                program << "    pop ax" << endl;
                program << "    cmp 0, ax" << endl;
                program << "    jump IF_" << tag << "_" << ++subIFS << "_END if EQU" << endl; // skip body if false

                createScope();

                word = lexer::to_upper( current.text );
                while( word != "END" and word != "FIN" and word != "ELSE" and word != "SINON" and current.type != STOP )
                {
                    parseStatement();
                    readEndl();

                    word = lexer::to_upper( current.text );
                }

                exitScope();

                program << "    jump IF_" << tag << "_END" << endl; // body has been executed
                program << ":IF_" << tag << "_" << subIFS << "_END" << endl;
            }
            else if( current.type == COLON )
            {
                readToken(); // read colon
                readEndl();

                program << "# ELSE " << endl;


                createScope();

                word = lexer::to_upper( current.text );
                while( word != "END" and word != "FIN" and current.type != STOP  )
                {
                    parseStatement();
                    readEndl();

                    word = lexer::to_upper( current.text );
                }
                exitScope();
                break; // avoid having else if after an else
            }
            word = lexer::to_upper( current.text );
        }

        word = lexer::to_upper( current.text );
        if( word != "END" and word != "FIN" )         
        {
            string mess = "Keyword 'END' expected after IF statement" ;
            if( frenchEnabled ) mess = "Mot clef 'FIN' attendu après bloc SI" ;
            throwCompileError( mess ); 
        }

        readToken(); // read END

        program << ":IF_" << tag << "_END" << endl;


    }

    // ForStatement := "FOR" Identifier|VarDeclaration "UNTIL" <var>Expression ':' ENDL {Statement} END
    void Compiler::parseForStatement( void )
    {
        readToken(); // read FOR keyword

        createScope();

        unsigned tag = ++tagNumber;
        string word = lexer::to_upper( current.text );
        string var_loop = "";

        unsigned index_buff = j;

        bool decl_def = false;  // true if declaring variable in the for ex: for var h=0 until 100:
                                //                                               ^^^
        if( current.type == TYPE )
        {
            parseVarDeclaration();
            var_loop = prev_assigned_var;
            decl_def = true;
        }
        else if( current.type == IDENTIFIER and tokens[j+1].type == EQU )
        {
            parseAssignement();
            var_loop = prev_assigned_var;
        }
        else if( current.type == IDENTIFIER )
        {
            parseFactor();
            program << "    pop" << endl;
            var_loop = prev_used_var;
        }

        Variable var = currentScope->getVar( var_loop );

        if( var.type != INT )
        {
            j = index_buff;
            string mess = "The variable for the loop 'for' must be of type 'var'" ;
            if( frenchEnabled ) mess = "La variable pour la boucle 'pour' doit etre de type 'var'" ;
            throwCompileError( mess );
        }

        word = lexer::to_upper( current.text );
        if( word != "UNTIL" and word != "JUSQUE" )
        {
            string mess = "Keyword 'UNTIL' expected" ;
            if( frenchEnabled ) mess = "Mot clef 'JUSQUE' attendu" ;
            throwCompileError( mess );
        }
        readToken();

        // code gen

        program << endl;
        program << "# For loop " << endl;
        program << ":FOR_" << tag << "_LOOP" <<  endl; 

        index_buff = j;
        Type type = parseExpression(); // target value is push on the stack
        if( type == BIN )
        {
            j = index_buff;
            string mess = "The target value in 'for' loop must be of type 'var' not 'bin'" ;
            if( frenchEnabled ) mess = "La valeur objectif de la boucle 'for' doit etre de type 'var', et non 'bin'" ;
            throwCompileError( mess );
        }


        readColon();
        readEndl();

        program << "    pop  bx     # target value in for loop" << endl;

        var = currentScope->getVar( var_loop );

        // code gen 

        accessVar( var ); // put var_loop address in ax
        
        program << endl;
        program << "    cmp  (ax), bx     # for loop comparison" << endl;
        program << "    jump FOR_" << tag << "_END  if EQU"   << endl; 
        program << "    jump FOR_" << tag << "_INCR if POS"   << endl; 
        program << "    jump FOR_" << tag << "_DECR if NEG"   << endl << endl; 
        
        program << "# for body" << endl;
        program << ":FOR_" << tag << "_BODY" << endl; 

        createScope();

        word = lexer::to_upper( current.text );
        while( word != "END" and word != "FIN" )
        {
            parseStatement();
            readEndl();

            word = lexer::to_upper( current.text );
        }

        exitScope();

        accessVar( var );

        program << "    pop  bx" << endl;
        program << "    add  bx, (ax)" << endl; 

        program << endl;
        program << "    jump FOR_" << tag << "_LOOP" << endl;

        program << ":FOR_" << tag << "_INCR" << endl; 
        program << "    push 1" << endl;
        program << "    jump FOR_" << tag << "_BODY"   << endl; 
        program << ":FOR_" << tag << "_DECR" << endl; 
        program << "    push -1" << endl;
        program << "    jump FOR_" << tag << "_BODY"   << endl;

        program << ":FOR_" << tag << "_END" << endl; 

        exitScope();



        word = lexer::to_upper( current.text );
        if( word != "END" and word != "FIN" )         
        {
            string mess = "Keyword 'END' expected after IF statement" ;
            if( frenchEnabled ) mess = "Mot clef 'FIN' attendu après bloc SI" ;
            throwCompileError( mess ); 
        }

        readToken();
    }

    // WhileStatement := "WHILE" <bool>Expression ':' ENDL {Statement} "END"
    void Compiler::parseWhileStatement( void )
    {
        readToken(); // read 'While'

        unsigned tag = ++tagNumber;

        program << endl;
        program << "# While loop " << endl;
        program << ":WHILE_" << tag << "_LOOP" << endl << endl;

        program << "# While condition" << endl;
    
        createScope();

        unsigned oj = j; // buffer current token index
        Type type = parseExpression(); // target value is push on the stack
        if( type != BIN )
        {
            j = oj;
            string mess = "The condition inside 'while' loop must be of type 'bin'" ;
            if( frenchEnabled ) mess = "La condition de la boucle 'tantque' doit être de type 'bin'" ;
            throwCompileError( mess );
        }

        program << "    pop  ax     # while comparison" << endl;
        program << "    cmp   0, ax" << endl;
        program << "    jump WHILE_" << tag << "_END if EQU" << endl << endl;

        program << "# While body" << endl;

        readColon();    // read ':'
        readEndl();     // read end of line

        
        string word = lexer::to_upper( current.text );
        while( word != "END" and word != "FIN" )
        {
            parseStatement();
            readEndl();

            word = lexer::to_upper( current.text );
        }

        exitScope();

        program << "    jump WHILE_" << tag << "_LOOP" << endl;
        program << ":WHILE_" << tag << "_END" << endl;

        word = lexer::to_upper( current.text );
        if( word != "END" and word != "FIN" )         
        {
            string mess = "Keyword 'END' expected after IF statement" ;
            if( frenchEnabled ) mess = "Mot clef 'FIN' attendu après bloc SI" ;
            throwCompileError( mess ); 
        }

        readToken();
        

    }

    // redirect to the corresponding function depending on the first token
    bool Compiler::parseStatement( void )
    {
        string word = lexer::to_upper( current.text );
        if( current.type == TYPE ) parseVarDeclaration();
        else if( current.type == KEYWORD )
        {
            if( word == "IF" or word == "SI" ) parseIfStatement();
            else if( word == "FOR" or word == "POUR" ) parseForStatement();
            else if( word == "WHILE" or word == "TANTQUE" ) parseWhileStatement();
            else
            {
                string mess = "Unrecognized instruction";
                if( frenchEnabled ) mess = "Instruction inconnue";
                throwCompileError( mess );
            } 
        }
        else if( current.type == RESERVED_FUNC )
        {
            if( word == "PRINT" or word == "IMPR" or word == "IMPRIMER" ) parseDISP();
        }
        else if( current.type == IDENTIFIER ) parseAssignement();
        else if( current.type == STOP ) return false;
        else if( current.type == ENDL ) return true;
        else if( current.text == "#" ) return true;
        else
        {
            string mess = "Unrecognized instruction";
            if( frenchEnabled ) mess = "Instruction inconnue";
            throwCompileError( mess );
        } 
        return true;
    }

    // create a new scope, both in assembly and in the compiler
    void Compiler::createScope( void )
    {
        // code gen
        program << endl;
        program << "# Entering new scope" << endl;
        program << "    push fx         # save old scope ref" << endl;
        program << "    copy sp, fx     # create current scope ref" << endl;
        program << "# Body of scope" << endl;
        // compiler side
        currentScope = new Scope( currentScope );   // create a new scope with current as parent
        // debuging
        // cout << "Creating Scope: " << currentScope << ", " << currentScope->parentScope << endl;
    }


    void Compiler::exitScope( void )
    {
        if( currentScope->parentScope != nullptr )
        { 
            // code gen
            program << endl;
            program << "# Exiting scope" << endl;
            program << "    copy fx, sp     # restore sp to current scope ref" << endl;
            program << "    pop  fx         # restore previous scope ref" << endl; 
            // compiler side
            Scope* s = currentScope;        // buffer
            currentScope = s->parentScope;  // leave current scope
            delete s;                       // delete left scope
        }
    }

    // put the addess of the var in ax register
    void Compiler::accessVar( Variable var ) 
    {
        program << "# Accessing var: " << var.name << endl;
        program << "    copy fx, ax" << endl;
        unsigned d = var.depth;
        while( d != 0 ) // access previous scopes if necessary
        {
            program << "    copy (ax), ax" << endl;
            d--;
        }
        program << "    add  " << var.offset << ", ax" << endl;
    }

    // display function
    void Compiler::parseDISP( void )
    {
        readToken(); // read disp

        if( current.type == QUOTES )
        {
            readToken(); // read quotes

            if( current.type != STRING )
            {
                string mess = "Expected a string of character";
                if( frenchEnabled ) mess = "Une chaine de charactere est attendu";
                throwCompileError( mess );
            }

            program << endl;
            program << "# displaying immediate string " << endl;
            bool esc = false;
            for( unsigned i=0; i<current.text.length(); i++ )
            {
                char c = current.text[i];
                string s{ c };
                if( c == '\\' )
                {
                    if( esc ) // already escaped
                    {
                        esc = false;
                        program << "    disp " << static_cast<uint16_t>( '\\' ) << " \t, char";
                        program << "      # disp '\\'" << endl;
                        continue;
                    }
                    esc = not esc;     
                    if( esc ) // if result is 
                        continue; // skip escaping char '\'
                }
                else
                {
                    if( esc )
                    {
                        if     ( c == 'n'  ) c = '\n';
                        program << "    disp " << static_cast<uint16_t>( c ) << " \t, char";
                        program << "      # disp '\\" << current.text[i] << "'" << endl;
                        esc = false;
                    }
                    else
                    {
                        program << "    disp " << static_cast<uint16_t>( c ) << " \t, char";
                        program << "      # disp '" << c << "'" << endl;
                    }
                }

            }
            readToken();
            if( current.type != QUOTES )
            {
                string mess = "Missing character '\"' in string";
                if( frenchEnabled ) mess = "Délimitant '\"' manquant à la chaine de charactere";
                throwCompileError( mess );
            }
            readToken();

        }
        else
        {
            Type type = parseExpression();
            string format = "";
            if     ( type == BIN )   format = "mem";
            else if( type == INT )   format = "int";
            else if( type == FLOAT ) format = "float";

            program << endl;
            program << "# displaying an expression" << endl;
            program << "    pop  ax" << endl;
            program << "    disp ax, " << format << endl;
        }

    }


    // return the corresponding string from basal Type
    string Compiler::getStringFromType( basal::Type type )
    {
        switch( type )
        {
            case INT:
                if( frenchEnabled ) return "entier";
                else                return "integer";
            case BIN:
                if( frenchEnabled ) return "binaire";
                else                return "binary";
            case FLOAT:
                return "decimal";

            case UNDECLARED:
                return "undeclared";
            case TYPE_ERROR:
                return "TYPE_ERROR";
            default:
                return "undefined";
        }
    }

}


