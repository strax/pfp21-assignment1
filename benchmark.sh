#!/bin/bash
set -eo pipefail

export BINDIR=./cmake-build-release

rm -f files/*.vb files/*.vb.dec
echo "** vbyteenc **"
command time -v ./bench/bench-vbyteenc.sh
echo ""

echo "** vbytedec **"
command time -v ./bench/bench-vbytedec.sh
echo ""

rm -f files/*.vb files/*.vb.dec
echo "** vbyteencsorted **"
command time -v ./bench/bench-vbyteencsorted.sh