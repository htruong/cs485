#!/bin/bash

./truongpqsort unit_test.txt > unit_test_truongpqsort.txt

sort unit_test.txt > unit_test_sort.txt

cmp -l unit_test_truongpqsort.txt unit_test_sort.txt

if [ $? -eq 0 ]; 
	then 
		echo "Unit test succeeded" 
	else 
		echo "Unit test failed"
fi

rm unit_test_truongpqsort.txt unit_test_sort.txt

