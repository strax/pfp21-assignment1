#!/bin/sh

for filename in files/*.vb; do
  "$BINDIR/vbytedec" "$filename"
done