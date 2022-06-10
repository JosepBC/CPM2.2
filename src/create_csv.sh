#!/bin/bash

echo "file_name;time;procs_per_node;nodes"
for file in out/dos6/*
do
    file_name=$(basename $file | cut -f 1 -d .)
    time=$(grep real $file | cut -f 2 -d ' ')
    procs=$(echo $file_name | cut -f 1 -d _)
    nodes=$(echo $file_name | cut -f 2 -d _)
    echo "$file_name;$time;$procs;$nodes"
done