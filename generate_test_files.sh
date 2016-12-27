#!/bin/bash

# File: generate_test_files.sh
# Author: Lennart Jern - ens16ljn@cs.umu.se
#
# Generate a file tree to do tests on.

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
    cd ..
done

# Big files
mkdir bigs
cd bigs

# Generate files with lots of zeros...
for F in $(seq 1 5); do
    head -c 1M < /dev/zero > "file$F"
done
