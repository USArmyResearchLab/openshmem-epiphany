#/bin/bash

n1="1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16"

n2="2 3 4 5 6 7 8 9 10 11 12 13 14 15 16"

p1="1 2 4 8 16"

p2="2 4 8 16"


run() 
{
	for n in $1; do
		echo coprsh -np $n -- $2
		coprsh -np $n -- $2
		echo ----------------------------------------
	done
}

run "$n1" ./hello.x
run "$p2" ./add.x
run "$n1" ./alltoall64.x
run "$n1" ./barrier.x
run "$n2" ./broadcast32.x
run "$n2" ./broadcast64.x
run "$n1" ./collect32.x
run "$n1" ./collect64.x
run "$p2" ./cswap_eq.x
run "$p2" ./cswap_neq.x
run "$p2" ./fadd.x
run "1 3 5 6 7 9 10 11 12 13 14 15" ./fcollect32.x
run "1 3 5 6 7 9 10 11 12 13 14 15" ./fcollect64.x
run "$p2" ./fetch.x
run "$p2" ./finc.x
run "$p1" ./get.x
run "$p1" ./get_ipi.x
run "$p1" ./get_nb.x
run "$p1" ./get_nb_dual.x
run "$p2" ./inc.x
run "$p1" ./put.x
run "$p1" ./put32.x
run "$p1" ./put64.x
run "$p1" ./put_nb.x
run "$p1" ./put_nb_dual.x
run "$p1" ./reduce.x
run "$p2" ./set.x
run "$p2" ./swap.x

