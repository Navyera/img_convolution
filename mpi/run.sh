#!/bin/bash


if (( $# < 2 )); then
	echo "Usage : ./run.sh machine_file prog_args"
	exit 1
fi

machine_file=$1
prog_args=${@:2}

mpiexec -f $machine_file ./img_conv $prog_args
