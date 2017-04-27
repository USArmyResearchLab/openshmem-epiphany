load 'gnuplot.style'
set border 11 back ls 11
set ytic auto
set title sprintf("Epiphany-III OpenSHMEM Reduction Performance (#PEs=%d)", PEs)
set xlabel "Number of Elements (nreduce)"
set ylabel "Latency (μsec)"
set y2label "Reductions/sec (millions)"
set key top left
set xr [1:2048]
set xtics ( 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, "1K" 1024, "2K" 2048 )
set logscale x 2
set logscale y 10
set log x2
unset log y2
set y2range [0:*]
set ytics nomirror
set y2tics
set style fill solid 0.15
plot "reduce.dat" using 1:($1*1000/$2) title 'Reductions/sec (millions)' with linespoints axes x2y2, \
 "reduce.dat" using 1:($2/1000) title 'Latency' with linespoints lt 2 lc rgb "blue"
