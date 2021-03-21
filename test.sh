#!/bin/bash
set -eo pipefail

rm -rf files/*.vb files/*.vb.dec

echo "Generating encoded files..."
for filename in files/*; do
  ./cmake-build-release/vbyteenc "$filename"
done

echo -n "Testing checksums..."
if shasum -s -a 512 -c checksums.encoded.txt; then
  echo " OK"
else
  echo " FAIL"
  exit 1
fi

echo "Generating decoded files..."
for filename in files/*.vb; do
  ./cmake-build-release/vbytedec "$filename"
done

echo -n "Testing checksums for decoded files..."
if shasum -s -a 512 -c checksums.decoded.txt; then
  echo " OK"
else
  echo " FAIL"
  exit 1
fi

echo -n "Check that input files have not changed..."
if shasum -s -a 512 -c checksums.inputs.txt; then
  echo " OK"
else
  echo " FAIL"
  exit 1
fi