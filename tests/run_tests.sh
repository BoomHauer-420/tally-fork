#!/usr/bin/env bash
# tests/run_tests.sh
# Usage: bash tests/run_tests.sh <path-to-tally-binary>
set -euo pipefail

TALLY="${1:-./build/tally}"
PASS=0
FAIL=0

# ── helpers ───────────────────────────────────────────────────────────────────

run_test() {
    local desc="$1"
    local expected="$2"
    local actual="$3"
    if [ "$actual" = "$expected" ]; then
        printf "  \033[32mPASS\033[0m  %s\n" "$desc"
        PASS=$((PASS + 1))
    else
        printf "  \033[31mFAIL\033[0m  %s\n" "$desc"
        printf "        expected: '%s'\n" "$expected"
        printf "        got:      '%s'\n" "$actual"
        FAIL=$((FAIL + 1))
    fi
}

expect_fail() {
    local desc="$1"
    if ! "$TALLY" "${@:2}" >/dev/null 2>&1; then
        printf "  \033[32mPASS\033[0m  %s\n" "$desc"
        PASS=$((PASS + 1))
    else
        printf "  \033[31mFAIL\033[0m  %s (expected non-zero exit)\n" "$desc"
        FAIL=$((FAIL + 1))
    fi
}

# ── fixtures ──────────────────────────────────────────────────────────────────

TMPDIR_LOCAL=$(mktemp -d)
trap 'rm -rf "$TMPDIR_LOCAL"' EXIT

printf "alpha\nbeta\ngamma\n"   > "$TMPDIR_LOCAL/three.txt"
printf ""                       > "$TMPDIR_LOCAL/empty.txt"
printf "no newline at end"      > "$TMPDIR_LOCAL/nonewline.txt"
printf "\n\n\n"                 > "$TMPDIR_LOCAL/blanklines.txt"

# ── tests ─────────────────────────────────────────────────────────────────────

echo "=== tally — line count ==="

count_of() { "$TALLY" "$1" | awk 'NR==1 {print $1; exit}'; }

run_test "three lines" \
    "3" "$("count_of" "$TMPDIR_LOCAL/three.txt")"

run_test "empty file gives 0" \
    "0" "$("count_of" "$TMPDIR_LOCAL/empty.txt")"

run_test "three blank lines" \
    "3" "$("count_of" "$TMPDIR_LOCAL/blanklines.txt")"

run_test "file with no trailing newline" \
    "0" "$("count_of" "$TMPDIR_LOCAL/nonewline.txt")"

run_test "output include filename" \
    "yes" "$("$TALLY" "$TMPDIR_LOCAL/three.txt | grep -qF "$TMPDIR_LOCAL/three.txt && echo yes)"

echo ""
echo "=== tally — error handling ==="

expect_fail "missing argument exits non-zero"
expect_fail "nonexistent file exits non-zero" "$TMPDIR_LOCAL/does_not_exist.txt"

# ── summary ───────────────────────────────────────────────────────────────────

echo ""
if [ "$FAIL" -gt 0 ]; then
    printf "\033[31mFAILED\033[0m  %d test(s) failed, %d passed\n" "$FAIL" "$PASS"
    exit 1
else
    printf "\033[32mOK\033[0m      %d test(s) passed\n" "$PASS"
fi
