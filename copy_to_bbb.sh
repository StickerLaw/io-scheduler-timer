#!/bin/bash

# File: copy_to_bbb.sh
# Author: Lennart Jern - ens16ljn@cs.umu.se

FILES="code/*.c code/*.h code/Makefile"

scp $FILES 192.168.1.11:~/Assignment3/code
