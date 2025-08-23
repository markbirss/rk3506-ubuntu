#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Usage: $0 <file_path>"
    exit 1
fi

file_path="$1"
if [ ! -f "$file_path" ]; then
    echo "Error: File does not exist!"
    exit 1
fi

file_size=$(stat --printf="%s" "$file_path")

aligned_size=$(( (file_size + 511) / 512 * 512 ))

if [ $file_size -ne $aligned_size ]; then
    truncate -s $aligned_size "$file_path"
    echo "File $file_path size modified to the aligned value ($file_size -> $aligned_size)."
else
    echo "File $file_path size is already aligned; no changes made."
fi
