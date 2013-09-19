#!/bin/bash
rm -rf output
echo "Serial nQueens:" | tee -a output

for i in {1..13} 
do 
	./nqueen 0 $i | grep sec | tee -a output
done

echo | tee -a output

echo "Parallel depth = 0:" | tee -a output
for i in {1..13}
do 
	./nqueen 1 $i 0 | grep sec | tee -a output
done

echo | tee -a output

echo "Parallel depth = 1:" | tee -a output
for i in {1..13} 
do 
	./nqueen 1 $i 1 | grep sec | tee -a output
done

echo | tee -a output

echo "Parallel depth = 2:" | tee -a output
for i in {1..13} 
do 
	./nqueen 1 $i 2 | grep sec | tee -a output
done
