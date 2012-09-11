#!/bin/bash

# run run.sh
./run.sh "ll1" "T42" "time"
./run.sh "ll0.5" "T42" "time" 
./run.sh "ll0.5" "gamil" "time" 
./run.sh "ll0.5" "licom" "time" 

exit 0
./run.sh "ll2.5" "ll0.5" "time" 
./run.sh "ll2.5" "ll1" "time" 
./run.sh "ll1" "ll0.5" "time" 
./run.sh "ll1" "ll2.5" "time" 
./run.sh "ll0.5" "ll1" "time" 
./run.sh "ll0.5" "ll2.5" "time" 
./run.sh "ll2.5" "T42" "time"
./run.sh "T42" "ll1" "time"
./run.sh "T42" "ll2.5" "time" 
./run.sh "T42" "ll0.5" "time"
./run.sh "T42" "gamil" "time"
./run.sh "T42" "licom" "time"
./run.sh "T42" "POP43" "time"
./run.sh "ll2.5" "gamil" "time"
./run.sh "ll2.5" "licom" "time"
./run.sh "ll2.5" "POP43" "time"
./run.sh "ll1" "gamil" "time"
./run.sh "ll1" "licom" "time"
./run.sh "ll1" "POP43" "time"

