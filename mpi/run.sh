#!/bin/bash

if [[ $@ < 1 ]] || ! [ "$1" -eq "$1" ] 2>/dev/null; then
    echo "Usage : ./run.sh num_procs prog_args"
    
    exit 1
fi

mpiexec -n $1 ./img_conv ${@:2}
