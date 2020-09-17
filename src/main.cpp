#include <iostream>
#include <chrono> 
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
        cout << "An Error has occured during compilation" << endl;
    }

    // end chrono
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    if( DISP_TIME ) 
        cout << "Assembled in " << elapsed.count() << " ms\n";

    cout << "Program:" << endl << compiler.program.str() << endl;
    for( uint32_t i=0; i<compiler.tokens.size(); i++)
    {
        if( compiler.tokens[i].type == basal::ENDL )
        {
            cout << endl;
        }
            cout << compiler.tokens[i].text << "|";
            cout << basal::getTokenTypeStr( compiler.tokens[i].type ) << ", ";

    }


    return 0;
}
