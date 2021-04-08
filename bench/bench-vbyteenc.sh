#!/bin/bash

shopt -s extglob

for filename in files/!(*.vb|*.dec); do
  "$BINDIR/vbyteenc" "$filename"
done