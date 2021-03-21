#!/bin/bash
set -eo pipefail

BINDIR=./cmake-build-release

run-encode-bench() {
  for filename in files/*; do
    "$BINDIR/vbyteenc" "$filename"
  done
}

run-decode-bench() {
  for filename in files/*.vb; do
    "$BINDIR/vbytedec" "$filename"
  done
}

rm -f files/*.vb files/*.vb.dec
echo "** vbyteenc **"
time run-encode-bench
echo ""

echo "** vbytedec **"
time run-decode-bench