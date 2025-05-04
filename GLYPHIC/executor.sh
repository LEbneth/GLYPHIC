#!/bin/bash

if [ "$#" -lt 1 ] || [ "$#" -gt 2 ]; then
    echo -e "Just to simplify command line execution for the GLYPHIC execuable.\n"
    echo -e "For terminal input, use an invalid filename.\n"
    echo -e "Usage: $0 <filename> [-d]"
    echo -e "   -d: Starts gdb with the file as args. Graphs are not generated."
    echo -e "   -g: Graphs are not compiled to png."
    exit 1
fi

FILENAME=$1
DEBUG_MODE=0
NO_GRAPH=0

if [ "$#" -eq 2 ]; then
    if [ "$2" == "-d" ]; then
        DEBUG_MODE=1
    elif [ "$2" == "-g" ]; then
        NO_GRAPH=1
    fi
fi

if [ $DEBUG_MODE -eq 1 ]; then
    clear
    gdb --args GLYPHIC "$FILENAME"
elif [ $GRAPH_MODE -eq 1 ]; then
    clear
    ./GLYPHIC "$FILENAME"
else
    clear
    ./GLYPHIC "$FILENAME"
    dot graphs/unoptimized.gv -Tpng -o graphs/unoptimized.png
    dot graphs/optimized.gv -Tpng -o graphs/optimized.png
fi