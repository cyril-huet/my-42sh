#!/bin/sh

PROGRAM=$1
TMP_DIR=$(mktemp -d "${TMPDIR:-/tmp}/my-42sh-test.XXXXXX")
EXPECTED_TMP=$(cd /tmp && pwd -P)

cleanup()
{
    rm -rf "$TMP_DIR"
}

fail()
{
    echo "[FAIL] $1"
    cleanup
    exit 1
}

check_output()
{
    expected=$1
    name=$2
    shift 2

    result=$("$@")

    if [ "$result" != "$expected" ]
    then
        echo "Expected: $expected"
        echo "Received: $result"
        fail "$name"
    fi

    echo "[OK] $name"
}

trap cleanup EXIT

if [ "$#" -ne 1 ]
then
    echo "Usage: $0 ./my-42sh"
    exit 1
fi

check_output "hello" "external command" \
    "$PROGRAM" "echo hello"

check_output "yes" "logical AND" \
    "$PROGRAM" "true && echo yes"

check_output "fallback" "logical OR" \
    "$PROGRAM" "false || echo fallback"

check_output "2" "pipeline" \
    "$PROGRAM" "echo one two | wc -w | tr -d ' '"

echo "hello" > "$TMP_DIR/input.txt"

"$PROGRAM" "cat < $TMP_DIR/input.txt" \
    > "$TMP_DIR/input-result.txt"

if [ "$(cat "$TMP_DIR/input-result.txt")" != "hello" ]
then
    fail "input redirection"
fi

echo "[OK] input redirection"

"$PROGRAM" "echo hello > $TMP_DIR/output.txt"

if [ "$(cat "$TMP_DIR/output.txt")" != "hello" ]
then
    fail "output redirection"
fi

echo "[OK] output redirection"

check_output "$EXPECTED_TMP" "cd builtin" \
    "$PROGRAM" "cd /tmp; pwd"

check_output "$EXPECTED_TMP" "subshell" \
    "$PROGRAM" "(cd /tmp; pwd)"

if "$PROGRAM" "echo hello >" > /dev/null 2>&1
then
    fail "invalid syntax"
fi

echo "[OK] invalid syntax"
echo "Integration tests passed"