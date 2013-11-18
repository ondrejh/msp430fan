#!/usr/bin/gnuplot

set term png
set output 'data.png'
set grid
set autoscale
set xlabel 't [min]'
set ylabel 'T [°C]'
#set y2tics
#set y2label 'pwr [N]'
set y2range [-0.5:10.5]
plot 'data.dat' i 0 t "t1" with lines axis x1y1,\
	 ''         i 1 t "t2" with lines axis x1y1,\
	 ''         i 2 t "mcu" with lines axis x1y1,\
	 ''			i 3 t "heating" with lines axis x1y2, \
	 ''			i 4 t "fuse" with lines axis x1y2
#pause -1
