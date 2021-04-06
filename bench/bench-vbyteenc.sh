#!/bin/sh

for filename in files/*; do
  "$BINDIR/vbyteenc" "$filename"
done