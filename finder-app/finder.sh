#!/bin/sh

writefile=$1
writestr=$2

if [ -z "$writefile" ] || [ -z "$writestr" ]; then
    echo "Error: Two arguments are required. Usage: $0 <directory> <search_string>"
    exit 1
fi

if [ ! -d "$writefile" ]; then
    echo "Error: The specified directory '$writefile' does not exist."
    exit 1
fi

num_files=$(find "$writefile" -type f | wc -l)
num_matching_lines=$(grep -r "$writestr" "$writefile" | wc -l)

echo "The number of files are $num_files and the number of matching lines are $num_matching_lines"
