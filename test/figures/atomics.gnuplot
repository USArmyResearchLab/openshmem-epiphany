load 'gnuplot.style'
set border 3 back ls 11
set title "Epiphany-III OpenSHMEM Atomic Operations Performance"
set xlabel "Number of Processing Elements"
set ylabel "Latency (μsec)"
set key outside
set yrange [0:*]
set style fill solid 0.15
plot \
 "atomic_compare_swap_neq.dat" using 1:($2/1000) title 'compare\_swap (!=)' with linespoints, \
 "atomic_fetch_add.dat"        using 1:($2/1000) title 'fetch\_add'         with linespoints, \
 "atomic_add.dat"              using 1:($2/1000) title 'add'                with linespoints, \
 "atomic_inc.dat"              using 1:($2/1000) title 'inc'                with linespoints, \
 "atomic_swap.dat"             using 1:($2/1000) title 'swap'               with linespoints, \
 "atomic_fetch_inc.dat"        using 1:($2/1000) title 'fetch\_inc'         with linespoints, \
 "atomic_compare_swap_eq.dat"  using 1:($2/1000) title 'compare\_swap (==)' with linespoints, \
 "atomic_fetch.dat"            using 1:($2/1000) title 'fetch'              with linespoints, \
 "atomic_set.dat"              using 1:($2/1000) title 'set'                with linespoints
