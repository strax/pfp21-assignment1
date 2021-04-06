#!/bin/sh

for filename in files/*; do
  "$BINDIR/vbyteencsorted" "$filename"
done