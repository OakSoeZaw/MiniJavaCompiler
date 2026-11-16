#!/bin/sh
BIN=build/minijava
for f in tests/semantic/*.java; do
    echo "=== $f"
    $BIN < "$f" 2>&1
    echo
done > tests/semantic/results.txt
cat tests/semantic/results.txt

for f in tests/base/*.java tests/extensions/*.java; do
    printf "%s: " "$f"
    build/minijava < "$f" 2>&1 | tail -1
done