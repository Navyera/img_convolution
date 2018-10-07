#!/bin/bash

procs=$1
procs_per_machine=$2

filename="machines"$procs"-"$procs_per_machine

if (( $# != 2 )); then
	echo "Bad number of arguments supplied..."
	exit 1
fi

if [ -f $filename ]; then
    rm $filename
fi

computers=$(( (procs + procs_per_machine - 1)/procs_per_machine ))
rem=$(( procs % procs_per_machine ))

for element in $(seq -w 01 $computers); do
    num="10#"$element

    #REMOVE THESE
    if (( num == 21)); then 
        continue
    fi
    #REMOVE ABOVE

    if (( num == computers && rem != 0)); then
        echo "linux"$element":"$rem >> $filename
    else 
        echo "linux"$element":"$procs_per_machine >> $filename
    fi
done


#REMOVE THESE 
if (( computers == 21 )); then
    echo "linux"$((computers+1))":"$rem >> $filename
elif (( computers > 21 )); then
    echo "linux"$((computers+1))":"$procs_per_machine >> $filename
fi
#REMOVE ABOVE

#echo "Created $filename!"
