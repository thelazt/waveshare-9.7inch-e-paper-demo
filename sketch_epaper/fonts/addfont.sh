#!/bin/bash
set -euo pipefail

MCUDIR=../mcufont/encoder
MCUFONT=mcufont
CHARS="0-255 0x2010-0x2015"
QUALITY=50
HEADER=../fonts.h

if [[ $# -ne 2 ]] ; then
	echo "Usage: $0 [Font] [Size]" >&2
	exit 1
else
	if [[ ! -x $MCUDIR/$MCUFONT ]] ; then
		make -C $MCUDIR $MCUFONT
	fi
	TTF=$1
	if [[ ! -f $TTF ]] ; then
		if fc-list | grep -i $TTF >/dev/null ; then
			TTF=$(fc-list | grep -i Arial | head -n1 | cut -d: -f1)
		else
			echo "Font $TTF not found!" >&2
			exit 1
		fi
	fi
	BASENAME="$(basename $TTF)"
	if [[ ! -f $BASENAME ]] ; then
		cp $TTF $BASENAME
	fi
	TARGET="${BASENAME%.*}$2.dat"
	$MCUDIR/$MCUFONT import_ttf $BASENAME $2
	if [[ ! -f $TARGET ]] ; then
		echo "Failed generating $TARGET!" >&2
		exit 1
	fi
	$MCUDIR/$MCUFONT filter $TARGET $CHARS
	$MCUDIR/$MCUFONT rlefont_optimize $TARGET $QUALITY
	$MCUDIR/$MCUFONT rlefont_export $TARGET
	SOURCE="${BASENAME%.*}$2.c"
	if [[ ! -f $SOURCE ]] ; then
		echo "Failed generating $SOURCE!" >&2
		exit 1
	fi
	echo "#include \"fonts/$SOURCE\"" >> $HEADER
	sort -u -o $HEADER $HEADER
	echo "Created $SOURCE!" >&2
fi

