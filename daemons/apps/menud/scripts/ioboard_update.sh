#!/bin/bash

PKGDIR=/home/robotino/.packages
	
MAJORREQUIRED=1
cat /proc/cpuinfo | egrep "(VIA Samuel 2)|(Geode\(TM\))." > /dev/null
if [ 0 -ne $? ] ; then
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
	exit 1
else
	echo $MAJORREQUIRED.$LATESTMINOR.$LATESTPATCH
fi

dpkg -i robotino-firmware_${MAJORREQUIRED}.${LATESTMINOR}.${LATESTPATCH}_i386.deb
