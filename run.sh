#!/bin/bash

make debug_bin

if [ $? -ne 0 ]; then
    echo $?
    exit 1
fi

for arg in "$@"
do
    ./debug_bin "$arg"
done

