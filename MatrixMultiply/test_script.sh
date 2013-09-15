#!/bin/bash

nRuns=5
if [ -z $1 ]
then
	dim=1024
else
	dim=$1
fi
output_fname=output-$dim
if [ -f $output_fname ]
then
	rm $output_fname
#	rm cpu_info
fi

echo "Matrix Multiply Test script begin @ $(date)" | tee -a $output_fname
startTime=$(date +'%s')
echo

#cat /proc/cpuinfo | grep "model name" | head -n 1 >> cpu_info
#cat /proc/cpuinfo | grep MHz >> cpu_info


for i in {2,6,3,7,8,5,1,4}
do
	./matmultpar $dim $i $nRuns	| tee -a $output_fname
done
echo >> $output_fname
echo >> $output_fname

./matmult $dim $nRuns | tee -a $output_fname

echo >> $output_fname

cat /proc/cpuinfo  >> $output_fname

echo >> $output_fname
endTime=$(date +'%s')
echo "$(echo "($endTime - $startTime) / 60" | bc -l ) minutes elapsed"| tee -a $output_fname
echo "Matrix Multiply Test script end @ $(date)" | tee -a $output_fname
