#!/bin/bash

function add_head {

if [ -z "$1" -o -z "$2" ] ; then
	echo "usage: add_head.sh file headfile"
	exit 1
fi

FILE="$1"
HEAD="$2"
FILENEW="$1--"

TMP=`mktemp`

L=`wc -l $HEAD | cut -f 1 -d " "`

head -n $L $FILE > $TMP

if diff $TMP $HEAD >/dev/null; then
	echo "skipping $FILE"
else
	mv $FILE $FILENEW
	cat $HEAD $FILENEW > $FILE
	rm $FILENEW
fi

rm $TMP
}

TODO_C=`find . -name \*.cc`
TODO_H=`find . -name \*.h`

for X in $TODO_C ; do
	add_head $X doc/head.cc
done

for X in $TODO_H ; do
	add_head $X doc/head.h
done

