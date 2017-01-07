#!/bin/bash

# File: generate_test_files.sh
# Author: Lennart Jern - ens16ljn@cs.umu.se
#
# Generate a file tree to do tests on.

# Starting directory
# START=/media/removable/KINGSTON
START=/run/media/lennart/Verbatim

# Move to correct directory/device
cd $START

# Remove tree if existent
rm -r test_files

# Create directory to hold all test files
mkdir test_files
cd test_files

# Split up the files between a few directories
for DIR in a b c d e; do
    mkdir $DIR
    cd $DIR
    # Create empy files
    for F in $(seq 1 20); do
        touch $F
    done

    for D in f g h i j; do
        mkdir $D
        cd $D
    done

    cd "$START/test_files"
done

# Big files
mkdir bigs
cd bigs

# Generate files with lots of zeros...
for F in $(seq 1 5); do
    head -c 50M < /dev/zero > "file$F"
done

cd "$START/test_files"
# Generate deep folders
for i in $(seq 1 10); do
    mkdir "deep$i"
    cd "deep$i"
    for D in $(seq 1 100); do
        mkdir "dir$D"
        cd "dir$D"
    done
    cd "$START/test_files"
done
