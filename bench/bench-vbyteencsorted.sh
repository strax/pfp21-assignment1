#!/bin/bash

shopt -s extglob

for filename in files/!(*.vb|*.dec); do
  "$BINDIR/vbyteencsorted" "$filename"
done