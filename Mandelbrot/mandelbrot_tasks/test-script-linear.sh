rm -rf ./output*
	runs=5
for j in $(seq 1 $runs )
do
	echo "Run #$j begin @ $(date)"
	fname=output-linear-$j
	for i in {512,448,384,320,256,192,128,64}
	do
		echo "Run #$j" >> $fname
		echo "Span = $i" >> $fname
		./mandelbrot-$i >> $fname
		echo >> fname
	done
	echo "Run #$j complete @ $(date)"
done
for i in {512,448,384,320,256,192,128,64}
do
	fname=output-$i
	cat $fname | grep -e Span -e speedup >> output_linear_combined
done


