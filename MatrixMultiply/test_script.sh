#!/bin/bash

if [ -f output ]
then
	rm output
	rm cpu_info
fi

cat /proc/cpuinfo | grep "model name" | head -n 1 >> cpu_info
cat /proc/cpuinfo | grep MHz >> cpu_info

nRuns=5
dim=1280

for i in {2,6,3,7,8,5,1,4}
do
	./matmultpar $dim $i $nRuns	| tee -a output
done
echo >> output
echo >> output

./matmult $dim $nRuns | tee -a output

cat cpu_info >> output
