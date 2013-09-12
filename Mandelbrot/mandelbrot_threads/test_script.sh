if [ -f output ]
then
	rm output
fi

for i in {2,6,3,7,8,5,1,4}
do
	./mandelbrot_threads $i | tee -a output
done

