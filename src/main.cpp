#include <iostream>
#include <chrono> 
#include <fstream>
#include "Compiler.h"
#include "BasalDef.h"

using std::cout; 
using std::cin ;
using std::cerr;
using std::endl;

const bool DISP_TIME = true;

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

    try // try to compile
    { 
        compiler.compile( file );
    } catch( const std::exception& e )
    {
        cerr << "An Error has occured during compilation" << endl;
        return 0;
    }

    // end chrono
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    if( DISP_TIME ) 
        cout << "Compiled  in " << elapsed.count() << " ms" << endl;

    string out = "out.basm";
    if( argc > 3 ) // output
    {
        string f{ argv[2] };
        if( f == "-o" )
            out = argv[3];
    }

    std::ofstream outfile( out );
    outfile << ":Program" << endl;
    outfile << compiler.program.str() << std::endl;
    outfile << "    disp (sp), int" << endl; // debugging
    outfile << "    EXIT" << endl;
    outfile.close(); 

    // cout << "Program:" << endl << compiler.program.str() << endl;
    // for( uint32_t i=0; i<compiler.tokens.size(); i++)
    // {
    //     if( compiler.tokens[i].type == basal::ENDL )
    //     {
    //         cout << endl;
    //     }
    //         cout << compiler.tokens[i].text << "|";
    //         cout << basal::getTokenTypeStr( compiler.tokens[i].type ) << ", ";
    // }


    return 0;
}
