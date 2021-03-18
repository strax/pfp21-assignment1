#!/bin/bash
set -eo pipefail

BIN=./cmake-build-release/vbyteenc

run-encode-bench() {
  for filename in files/*; do
    $BIN "$filename"
  done
}

rm -f files/*.vb files/*.vb.dec
time run-encode-bench