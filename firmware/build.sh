#!/usr/bin/env bash

set -euo pipefail

environment=${1:-}
if [[ $# -ne 1 || ( $environment != "esp32" && $environment != "esp32s3" ) ]]; then
    echo "Usage: $0 <esp32|esp32s3>" >&2
    exit 2
fi

pio run -d firmware -e "$environment" -t compiledb
pio run -d firmware -e "$environment" -t upload
