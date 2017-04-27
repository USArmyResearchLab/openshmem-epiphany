load 'gnuplot.style'
set border 3 back ls 11
set title "Epiphany-III OpenSHMEM Atomic Operations Performance"
set xlabel "Number of Processing Elements"
set ylabel "Latency (μsec)"
set key outside
set xrange [2:*]
set style fill solid 0.15
plot \
 "cswap_neq.dat" using 1:($2/1000) title 'cswap (!=)' with linespoints, \
 "fadd.dat"      using 1:($2/1000) title 'fadd'       with linespoints, \
 "add.dat"       using 1:($2/1000) title 'add'        with linespoints, \
 "inc.dat"       using 1:($2/1000) title 'inc'        with linespoints, \
 "swap.dat"      using 1:($2/1000) title 'swap'       with linespoints, \
 "finc.dat"      using 1:($2/1000) title 'finc'       with linespoints, \
 "cswap_eq.dat"  using 1:($2/1000) title 'cswap (==)' with linespoints, \
 "fetch.dat"     using 1:($2/1000) title 'fetch'      with linespoints, \
 "set.dat"       using 1:($2/1000) title 'set'        with linespoints
