#! /bin/bash
for h in 2 4 8 16 32
do
    for j in {1..10}
    do
        for i in 10 100 200 400 600 800 1000 2000
        do
            ./procesos $i $h >> timesProcesos.doc
        done
    done
done