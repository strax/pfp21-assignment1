#!/bin/bash
set -eo pipefail
shopt -s extglob

rm -rf files/*.vb files/*.vb.dec

echo "Generating encoded files..."
for filename in files/!(*.vb|*.dec); do
  ./cmake-build-release/vbyteenc "$filename"
done

echo -n "Testing checksums..."
if sha512sum --quiet --check checksums.encoded.txt; then
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
if sha512sum --quiet --check checksums.decoded.txt; then
  echo " OK"
else
  echo " FAIL"
  exit 1
fi

echo -n "Check that input files have not changed..."
if sha512sum --quiet --check checksums.inputs.txt; then
  echo " OK"
else
  echo " FAIL"
  exit 1
fi

echo "Generating encoded files with delta compression..."
for filename in files/!(*.vb|*.dec); do
  ./cmake-build-release/vbyteencsorted "$filename"
done

echo -n "Testing checksums..."
if sha512sum --quiet --check checksums.sorted.txt; then
  echo " OK"
else
  echo " FAIL"
  exit 1
fi

echo -n "Check that input files have not changed..."
if sha512sum --quiet --check checksums.inputs.txt; then
  echo " OK"
else
  echo " FAIL"
  exit 1
fi