#!/bin/sh
BIN=build/minijava
PASS=0
FAIL=0

mkdir -p tests/results

check() {
    f="$1"
    expect="$2"
    out="$3"
    result=$($BIN < "$f" 2>&1)
    {
        echo "=== $f"
        echo "$result"
        echo
    } >> "$out"
    lastline=$(echo "$result" | tail -1)
    if echo "$lastline" | grep -q "$expect"; then
        PASS=$((PASS+1))
    else
        FAIL=$((FAIL+1))
        echo "FAIL: $f"
        echo "      expected: $expect"
        echo "      got:      $lastline"
    fi
}

# 1. base samples: must parse and pass semantics
: > tests/results/base.txt
for f in tests/base/*.java; do
    [ -e "$f" ] && check "$f" "^Parse successful" tests/results/base.txt
done

# 2. extension samples: same expectation
: > tests/results/extensions.txt
for f in tests/extensions/*.java; do
    [ -e "$f" ] && check "$f" "^Parse successful" tests/results/extensions.txt
done

# 3. semantic tests: 16_valid must succeed, all others give exactly 1 error
: > tests/results/semantic.txt
for f in tests/semantic/*.java; do
    [ -e "$f" ] || continue
    case "$f" in
        *16_valid*) check "$f" "^Parse successful" tests/results/semantic.txt ;;
        *)          check "$f" "^Semantic check completed with 1 errors" tests/results/semantic.txt ;;
    esac
done

echo
echo "Passed: $PASS   Failed: $FAIL"
echo "Saved: tests/results/base.txt, tests/results/extensions.txt, tests/results/semantic.txt"
[ "$FAIL" -eq 0 ]
