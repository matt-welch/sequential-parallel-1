if [ -f output ]
then
	rm output
	rm cpu_info
fi

cat /proc/cpuinfo | grep "model name" | head -n 1 >> cpu_info
cat /proc/cpuinfo | grep MHz >> cpu_info


for i in {2,6,3,7,8,5,1,4}
do
	./mandelbrot_threads $i | tee -a output
done

echo >> output
cat cpu_info >> output
