#/bin/bash

n1="1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16"

run() 
{
	for n in $1; do
		echo coprsh -np $n -- $2
		coprsh -np $n -- $2
		echo ----------------------------------------
	done
}

run "$n1" ./hello.x
run "$n1" ./add.x
run "$n1" ./alltoall64.x
run "$n1" ./barrier.x
run "$n1" ./broadcast32.x
run "$n1" ./broadcast64.x
run "$n1" ./collect32.x
run "$n1" ./collect64.x
run "$n1" ./cswap_eq.x
run "$n1" ./cswap_neq.x
run "$n1" ./fadd.x
run "$n1" ./fcollect32.x
run "$n1" ./fcollect64.x
run "$n1" ./fetch.x
run "$n1" ./finc.x
run "$n1" ./get.x
run "$n1" ./get_ipi.x
run "$n1" ./get_nb.x
run "$n1" ./get_nb_dual.x
run "$n1" ./inc.x
run "$n1" ./put.x
run "$n1" ./put32.x
run "$n1" ./put64.x
run "$n1" ./put_nb.x
run "$n1" ./put_nb_dual.x
run "$n1" ./reduce.x
run "$n1" ./set.x
run "$n1" ./swap.x
