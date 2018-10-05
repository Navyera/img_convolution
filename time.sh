#!/bin/bash


if (( $# != 6 )); then
	echo "Bad number of arguments supplied..."
	exit 1
fi

width=$1
height=$2
color=$3
proc_limit=$4
procs_per_machine=$5
subdir=$6


LOOPS=30 #CHANGE THIS IF NEEDED



prog_args="-w "$width" -h "$height" -c "$color" -l "$LOOPS

times=""

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



