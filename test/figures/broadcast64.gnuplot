load 'gnuplot.style'
set border 11 back ls 11
set format y "%0.1f"; set ytic auto
fit Tc(x) 'broadcast64.dat' using 1:2 via alpha, beta
set title sprintf("Epiphany-III OpenSHMEM Broadcast64 Performance (Tree)\n{/*0.9 α = %.0f ± %.0f nsec, β^{-1} = %.3f ± %.3f GB/s}", alpha, alpha_err, 1/beta, beta_err/beta)
set xlabel "Message Size (bytes)"
set ylabel "Latency (μsec)"
set y2label "Calculated Total On-Chip Bandwidth (GB/s)"
set key top left
set xr [8:8192]
set logscale x 2
set xtics ( 8, 16, 32, 64, 128, 256, 512, "1K" 1024, "2K" 2048, "4K" 4096, "8K" 8192 )
set logscale y 10
set log x2
unset log y2
set y2range [0:*]
set ytics nomirror
set y2tics
set style fill solid 0.15
plot "broadcast64.dat" using 1:(PEs*$1/$2) title 'Bandwidth' with linespoints axes x2y2, \
 "broadcast64.dat" using 1:($2/1000) title 'Latency' with linespoints lt 2 lc rgb "blue"
