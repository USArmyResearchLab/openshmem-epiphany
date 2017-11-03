load 'gnuplot.style'
set border 3 back ls 11
set format y "%0.2f"; set ytic auto
set title "Epiphany-III OpenSHMEM Barrier Performance (Dissemination)"
set xlabel "Number of Processing Elements"
set ylabel "Latency (μsec)"
set key bottom right
set xr [1:16]
set style circle
plot "barrier.dat" using 1:($2/1000) title 'Runtime w/ variable #PEs' with linespoints,
