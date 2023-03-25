#! /bin/bash
for j in {1..10}
    do
        for i in 10 100 200 400 600 800 1000 2000
        do
            ./hilos $i 4 >> timesHilosWithO.doc
        done
    done
