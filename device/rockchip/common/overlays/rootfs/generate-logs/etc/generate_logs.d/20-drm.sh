#!/bin/sh

cd "${1:-/tmp/}"

type modetest >/dev/null 2>/dev/null || exit 0
modetest -a > modetest.txt

type kmsgrab >/dev/null 2>/dev/null || exit 0
for p in $(modetest -p | grep -A 1000 "^P" | grep -o "^[0-9]*"); do
	DUMP=plane_$p
	kmsgrab --plane $p >$DUMP.raw 2>$DUMP.txt
	[ "$(wc -c $DUMP.txt | cut -d' ' -f1)" -ne 0 ] || rm -rf $DUMP.*
done
