#!/bin/bash

# timer.sh
#
# A timer script to measure the differences between i/o schedulers
#
# Author: Lennart Jern (ens16ljn@cs.umu.se)

# Clean up old results
rm ../data/*.log

SCHEDULERS="cfq noop deadline"
DEVICE="/sys/block/sdc/queue/scheduler"
# Starting directory and expression to search for
START="/run/media/lennart/KINGSTON/test_files expression"
# START="/run/media/lennart/Verbatim/test_files expression"
MNT="/run/media/lennart/KINGSTON"

for S in $SCHEDULERS
do
    echo $S | sudo tee $DEVICE
    echo "Scheduler: `cat $DEVICE`"
    # LINE=""
    # Time the commands 10 times
    for i in $(seq 1 10)
    do
        # Unmount and mount to clear all cache
        sudo umount $MNT
        sudo rm -d /run/media/lennart/KINGSTON
        sudo mkdir /run/media/lennart/KINGSTON
        sudo mount /dev/sdc1 $MNT

        # We use 4 parallell comands that store their respective times in
        # separate log files
        COMMAND1="./mfind $START >> ../data/$S-1.log"
        COMMAND2="./mfind $START >> ../data/$S-2.log"
        COMMAND3="./mfind $START >> ../data/$S-3.log"
        COMMAND4="./mfind $START >> ../data/$S-4.log"

        eval $COMMAND1 &
        eval $COMMAND2 &
        eval $COMMAND3 &
        eval $COMMAND4 &
        # Wait for all commands to finish
        wait

        # A little progress report
        echo "Run $i done."
    done

done

# Restore cfq scheduler
echo "cfq" | sudo tee $DEVICE
