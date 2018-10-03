#!/bin/bash


if (( $# != 4 )); then
	echo "Bad number of arguments supplied..."
	exit 1
fi

input_file=$(realpath $1)

file_metadata=$(echo $1 | rev | cut -d "." -f 2 | rev)
proc_limit=$2
procs_per_machine=$3
subdir=$4

LOOPS=30 #CHANGE THIS IF NEEDED

color=$(echo $file_metadata | cut -d "_" -f 2)
color=${color^^}

width=$(echo $file_metadata | cut -d "_" -f 3)
height=$(echo $file_metadata | cut -d "_" -f 4)

prog_args="-f "$input_file" -w "$width" -h "$height" -c "$color" -l "$LOOPS

output_filename="results.csv"



for (( i=1; i*i <= proc_limit; i++ )); do
    bash machines.sh $((i*i)) $procs_per_machine

	cd $subdir

	time=$(bash run.sh ../machines$((i*i))-$procs_per_machine $prog_args | cut -d ' ' -f 8)

	if (( (i+1)*(i+1) > proc_limit )); then
		echo -n $time >> $output_filename
	else
		echo -n $time", " >> $output_filename 
	fi
	cd ..
done

echo "" >> $subdir/$output_filename
mv $subdir/$output_filename .

rm -f machines*-*


#echo $input_file $color $width $height $proc_limit



