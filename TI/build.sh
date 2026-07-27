#!/usr/bin/env bash

set -euo pipefail

cd "$(dirname "$0")"

case "${1:-app}" in
    app) target=flash ;;
    smoke) target=flash-smoke ;;
    smoke-build) target=smoke ;;
    smoke-flash)
        exec uvx --from pyocd pyocd load -t MSPM0G3507 \
            project/gcc/build/smoke/seekfree.elf
        ;;
    *) echo "usage: $0 [app|smoke|smoke-build|smoke-flash]" >&2; exit 2 ;;
esac

make -C project/gcc setup "$target"
