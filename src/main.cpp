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
    bool s = compiler.compile( file );

    // end chrono
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    if( DISP_TIME ) 
        cout << "Assembled in " << elapsed.count() << " ms\n";

    for( uint32_t i=0; i<compiler.tokens.size(); i++)
    {
        if( compiler.tokens[i].type == basal::ENDL )
        {
            cout << endl;
        }
        else
            cout << basal::getTokenTypeStr( compiler.tokens[i].type ) << " \'" << compiler.tokens[i].text << "\' " << endl;
    }


    // terminate program if assemble returned false
    if( s == false )
    {
        cerr << "A problem has occured while compiling file '" << file << "'." << endl;
        exit( -1 );
    }

    return 0;
}
