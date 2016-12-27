#!/bin/bash

# timer.sh
#
# A timer script to measure the differences between i/o schedulers
#
# Author: Lennart Jern (ens16ljn@cs.umu.se)

# Clean up old results
rm ../data/data.csv
rm ../data/*.log

SCHEDULERS="cfq noop deadline"

for S in cfq
do
    echo "Running with $S scheduler"
    LINE=""
    # Time the commands 10 times
    for i in $(seq 1 10)
    do
        FLAGS="../test_files expression"
        COMMAND="./mfind $FLAGS >> ../data/$S.log"
        # Run the command and store the time
        t="$(sh -c "TIMEFORMAT='%5R'; /usr/bin/time -f '%e' $COMMAND" 2>&1)"
        LINE="$LINE,$t"

        # A little progress report
        echo "Run $i done."
    done
    # There is already a comma first in LINE
    DATA=$S$LINE
    # Write data to a file
    echo "$DATA" >> "../data/data.csv"

done
