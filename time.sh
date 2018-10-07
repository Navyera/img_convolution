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

	sum=0
	for (( j=0; j<5; j++ )); do  #CHANGE THIS FOR LOOP TO GET MORE ACCURATE AVERAGES
		temp=$(bash run.sh ../machines$((i*i))-$procs_per_machine $prog_args)
		sum=$( echo "$sum + $temp" | bc -l | awk '{printf "%.6f\n", $0}' )
	done

	time=$(echo "$sum / 5" | bc -l | awk '{printf "%.6f\n", $0}' )

	if (( (i+1)*(i+1) > proc_limit )); then
		times+=$time
	else
		times+=$time", "
	fi
	cd ..
done

rm -f machines*-*


echo $times




