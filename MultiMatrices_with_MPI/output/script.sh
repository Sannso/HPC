#! /bin/bash
for j in {1..10}
do
    for i in 10 12 13
    do
        ./secuencialMPI $i 0 >> timesSecuencial.doc
    done
done