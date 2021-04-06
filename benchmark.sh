#!/bin/bash
set -eo pipefail

export BINDIR=./cmake-build-release

rm -f files/*.vb files/*.vb.dec
echo "** vbyteenc **"
env time -v ./bench/bench-vbyteenc.sh
echo ""

echo "** vbytedec **"
env time -v ./bench/bench-vbytedec.sh
echo ""

rm -f files/*.vb files/*.vb.dec
echo "** vbyteencsorted **"
env time -v ./bench/bench-vbyteencsorted.sh