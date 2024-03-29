#include <iostream>
#include <chrono> 
#include <fstream>
#include "Compiler.h"
#include "BasalDef.h"

using std::cout; 
using std::cin ;
using std::cerr;
using std::endl;

const bool DISP_TIME   = true;
const bool DISP_TOKENS = false;

int main( int argc, char *argv[] )
{

    if( argc < 2 )
    {
        cerr << "No target to assemble. Terminating program." << endl;
        exit( -1 );
    }

    string file = argv[1];

    // string file = "";
    // cout << "load: ";
    // cin >> file;

    // start chrono 
    auto start = std::chrono::high_resolution_clock::now();

    // Instanciate Assambler, assemble instructions
    basal::Compiler compiler;

    string out = "out.basm";
    if( argc >= 4 ) // output
    {
        cout  << " OK " << endl;
        string s{ argv[2] };
        if( s == "-o" )
        {
            string of = {argv[3]};
            out = of;
        }
    }


    try // try to compile
    { 
        compiler.compile( file );
    } catch( ... )
    {
        exit(-1);
    }

    // end chrono
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    if( DISP_TIME ) 
        cout << "Compiled  in " << elapsed.count() << " ms" << endl;

    std::ofstream outfile( out );
    outfile << ":Program" << endl;
    outfile << "    disp 10, char" << endl;
    outfile << compiler.program.str() << std::endl;

    outfile << "    EXIT" << endl;
    outfile.close(); 

    // cout << "Program:" << endl << compiler.program.str() << endl;

    if( DISP_TOKENS )
    {    
        for( uint32_t i=0; i<compiler.tokens.size(); i++)
        {
            if( compiler.tokens[i].type == basal::ENDL )
            {
                cout << endl;
            }
                cout << "[" << compiler.tokens[i].text << "|";
                cout << basal::getStringFromTokenType( compiler.tokens[i].type ) << "] ";
        }
        cout << endl;
    }

    return 0;
}
