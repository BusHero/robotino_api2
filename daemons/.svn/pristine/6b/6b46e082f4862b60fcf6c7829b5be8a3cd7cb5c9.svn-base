#!/bin/bash

ROBTOPIC="/usr/local/robotino/common/bin/robtopic"
PKGDIR=/home/robotino/.packages

ret=`$ROBTOPIC -timeout=1000 echo ea_version`
if [ 0 -eq $? ] ; then
	board=`echo $ret | gawk '{print $4}'`
	version=`echo $ret | gawk '{print $5}'`
	echo "$board"
	echo Current firmware
	echo "$version"
	if [ "ea05" == "$board" ] ; then
		echo Update not
		echo available for
		echo EA05 board
		exit 0
	fi
	
	MAJORREQUIRED=1
	if [ "ea12" == "$board" ] ; then
		MAJORREQUIRED=2
	fi

	echo "New firmware"
	LATESTMINOR=0
	LATESTPATCH=0
	UPDATEFOUND=0

	if [ -e "$PKGDIR" ] ; then
		shopt -s nullglob
		cd $PKGDIR
		FILES=robotino-firmware_*_i386.deb
		for f in $FILES
		do
			#echo $f
			MAJOR=`echo $f | gawk -F'[_.]' '{print $2}'`
			MINOR=`echo $f | gawk -F'[_.]' '{print $3}'`
			PATCH=`echo $f | gawk -F'[_.]' '{print $4}'`
			#echo $MAJOR-$MINOR-$PATCH
			if [ $MAJOR -eq $MAJORREQUIRED ] ; then
				if [ $MINOR -gt $LATESTMINOR ] ; then
					LATESTMINOR=$MINOR
					LATESTPATCH=$PATCH
					UPDATEFOUND=1
				elif [ $MINOR -eq $LATESTMINOR ] ; then
					if [ $PATCH -gt $LATESTPATCH ] ; then
						LATESTPATCH=$PATCH
					fi
				fi
			fi
		done
	fi
	if [ 0 -eq $UPDATEFOUND ] ; then
		echo "no update"
		echo "package found"
	else
		echo $MAJORREQUIRED.$LATESTMINOR.$LATESTPATCH
	fi
	exit 0
else
	echo "robtopic error"
fi

exit 1
