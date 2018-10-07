#!/bin/bash

if (( $# != 7 )); then
	echo "Usage : ./bench.sh base_width base_height max_div max_mult proc_limit procs_per_machine  subdir"
	exit 1
fi



base_width=$1
base_height=$2
max_div=$3
max_mult=$4
proc_limit=$5
procs_per_machine=$6
subdir=$7

#Recompile for safety
cd $subdir
make
make clean
cd ..

colors=(GRAY RGB)


for color in "${colors[@]}"; do

    if [[ -f "results_"$color"_"$subdir".csv" ]]; then
        rm -f "results_"$color"_"$subdir".csv"
    fi

    


    for (( i=$max_div; i > 1; i--)); do
        cur_width=$(( base_width/i ))
        line=$(bash time.sh $cur_width $base_height $color $proc_limit $procs_per_machine $subdir)
        echo $line >> "results_"$color"_"$subdir".csv"
    done

    for (( i=1; i <= $max_mult; i++)); do
        cur_width=$(( base_width * i ))
        line=$(bash time.sh $cur_width $base_height $color $proc_limit $procs_per_machine $subdir)
        echo $line >> "results_"$color"_"$subdir".csv"
    done
done

