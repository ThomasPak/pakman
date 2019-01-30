#!/bin/sh

# Save command and shift argument
cmd="$1"
shift

# Execute command while discarding stderr
"$cmd" "$@" 2>/dev/null
