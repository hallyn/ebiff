#!/bin/sh

#$1 from
#$2 to

#dirs
FILES=`find . -type d -name \*$1\*`
while [ ! -z "$FILES" ]; do
	for X in $FILES; do
		Y=`echo "$X" | sed s?$1?$2?`
		#echo mv "$X" "$Y"
		mv "$X" "$Y"
		break
	done
	FILES=`find . -type d -name \*$1\*`
done

#files
FILES=`find . -type f -name \*$1\*`
for X in $FILES; do
	Y=`echo "$X" | sed s?$1?$2?`
	#echo mv "$X" "$Y"
	mv "$X" "$Y"
done

#files
FILES1=`find . -type f -name \*.cc`
FILES2=`find . -type f -name \*.h`
FILES3=`find . -type f -name \*.lua`
FILES4=`find . -type f -name \*.xslt`
FILES5=`find . -type f -name Makefile`
FILES6=`find debian -type f -name \*`
FILES="$FILES1 $FILES2 $FILES3 $FILES4 $FILES5 $FILES6"
for X in $FILES; do
	Y=`mktemp`
	
	cat $X | sed s?$1?$2?g > $Y

	#echo mv $Y $X
	mv $Y $X

	rm -f $Y
done
