#!/bin/sh

set -e

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 MAX_ERRORS MAX_WARNINGS" >&2
    exit 1
fi

MAX_ERRORS=$1
MAX_WARNINGS=$2

# Use or true because the script should continue even if cppcheck fails
cppcheck --check-level=exhaustive src 2> check.log || true

ERRORS=$( cat check.log | grep -c "error:" )
WARNINGS=$( cat check.log | grep -c "warning:" )

if [ "$ERRORS" -gt "$MAX_ERRORS" ]; then
    cat check.log
    echo "Wanted $MAX_ERRORS errors, but found $ERRORS"
    false
else

    if [ "$WARNINGS" -gt "$MAX_WARNINGS" ]; then
        cat check.log
        echo "Wanted $MAX_WARNINGS warnings, but found $WARNINGS"
        false
    fi
fi
