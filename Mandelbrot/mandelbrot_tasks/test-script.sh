rm -rf ./output*
	runs=5
for j in $(seq 1 $runs )
do
	fname=output-$j
	for i in {4,32,512,128,1,8,256,2,16,64}
	do
		echo "Run #$j" >> $fname
		echo "Span = $i" >> $fname
		./mandelbrot-$i >> $fname
		echo >> fname
	done
done
for i in {4,32,512,128,1,8,256,2,16,64}
do
	fname=output-$i
	cat $fname | grep -e Span -e speedup >> output_combined
done



#3, 6, 10, 8, 1, 4, 9, 2, 5, 7

