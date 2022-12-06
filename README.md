# Basal Lang

This is a polyglot language (english and french) that runs on a 16bit VM called BasalVM.

# Features

    - Variables
        [x] integer  (int, integer, entier, ent)
        [x] boolean  (bin, binary, binaire)
        [x] float [16bit] (dec, decimal)
        [ ] strings (not done)
        [ ] char
        [ ] arrays
        
    - Control Flow
        [x] If/Else statement
        [x] For loops
        [x] While loops
    
    - Print Statement
    
    - Abstractions
        [ ] Functions
        [ ] Classes
        
        
    [ ] Heap Allocation
        
# Dependencies
	- gcc
    - make
    
# Build
    make

# Usage

Call the compile to a Basal Lang file, the compiler will out the assembly (basalVM) equivalent of the program, if no error was encountered.
You can change the file output with the -o option. 

./main <basm_file>

	ex : 	./bin/bsl examples/fibo.bsl -o fibo.basm

# Example

    
    # English keywords

    integer a = 0
    integer b = 1

    print a

    for integer i=0 until 10:
        print ", "
        b = a + b
        a = b - a
        print a
    end

    print "\n------------\n"

    # French keywords

    entier c = 0
    entier d = 1

    impr c

    pour entier i=0 jusque 10:
        impr ", "
        d = d + c
        c = d - c
        impr c
    fin

    impr "\n"