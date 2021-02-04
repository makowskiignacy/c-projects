#!/bin/bash
prog="$1"
dir="$2"
for i in ${dir}/*.in; do
	echo ${i}
	filename="${i%.in}"
	valgrind --error-exitcode=15 --log-file="valgrind.txt" --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all ${prog} < ${filename}.in 1>test.out 2>test.err
	diff test.out ${filename}.out
	sprawdzian_out="$?"
	diff test.err ${filename}.err
	sprawdzian_err="$?"
	cat valgrind.txt
	if [[ $sprawdzian_out -eq "0" ]] && [[ $sprawdzian_err -eq "0" ]]; then
		echo "TEST ZAKOŃCZONY POWODZENIEM"
	else
  	echo "TEST ZAKOŃCZONY NIEPOWODZENIEM"
	fi
	rm -f test.out test.err valgrind.txt
done
