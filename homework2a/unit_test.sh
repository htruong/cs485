#!/bin/bash

$1 $2 > unit_test_truongpqsort.txt

sort $2 > unit_test_sort.txt

cmp -l unit_test_truongpqsort.txt unit_test_sort.txt

if [ $? -eq 0 ];
    then
        echo "Unit test for $2 succeeded"
    else
        echo "Unit test for $2 failed"
fi

rm unit_test_truongpqsort.txt unit_test_sort.txt

