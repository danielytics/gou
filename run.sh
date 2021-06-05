#!/bin/sh
VARIANT=$1

if [ "$VARIANT" != "debug" ] && [ "$VARIANT" != "debug-with-asan" ] && [ "$VARIANT" != "dev" ] && [ "$VARIANT" != "release" ]; then
	echo "Usage: ./run.sh <variant> ...args"
	echo "    where <variant> is one of: debug debug-with-asan dev release"
	echo "    ...args if present are passed to the gou binary"
	echo
	exit
else
	BINARY=gou-$VARIANT
	if [ "$VARIANT" = "release" ]; then
		BINARY=gou
	fi
	shift
	cd build-$VARIANT && ./$BINARY --gamefiles ../game --gamefiles ../common $@
fi
