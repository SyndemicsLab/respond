#!/usr/bin/bash -l

if [[ -f "bin/respond" ]]; then
    
    read -p "Enter the path, first, and last input set: " arg1 arg2 arg3
    ./bin/respond $arg1 $arg2 $arg3
else
    echo "RESPOND executable not found! Exiting..."
fi
