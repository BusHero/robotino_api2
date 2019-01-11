#!/bin/bash

CURRENT=$1
SCRIPT=/usr/local/robotino/firmware/install.sh

echo Installed version is $CURRENT

VERSIONSTR=`grep ^VERSION=[0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*$ $SCRIPT`

echo $VERSIONSTR
VERSIONSTR=`echo $VERSIONSTR | sed 's/VERSION=//'`

C0=`echo $CURRENT | gawk 'BEGIN { FS = "." } ; { print $1 }'`
C1=`echo $CURRENT | gawk 'BEGIN { FS = "." } ; { print $2 }'`
C2=`echo $CURRENT | gawk 'BEGIN { FS = "." } ; { print $3 }'`

P0=`echo $VERSIONSTR | gawk 'BEGIN { FS = "." } ; { print $1 }'`
P1=`echo $VERSIONSTR | gawk 'BEGIN { FS = "." } ; { print $2 }'`
P2=`echo $VERSIONSTR | gawk 'BEGIN { FS = "." } ; { print $3 }'`

C0=$(($C0+0))
C1=$(($C1+0))
C2=$(($C2+0))

P0=$(($P0+0))
P1=$(($P1+0))
P2=$(($P2+0))

echo Current $C0 $C1 $C2
echo Package $P0 $P1 $P2

DOUPDATE=0

if (( $P0 > $C0 )) ; then
  DOUPDATE=1
  echo "P0>C0 $P0>$C0"
elif (( $P0 == $C0 )) ; then
  if (( $P1 > $C1 )) ; then
    DOUPDATE=1
    echo "P1>C1 $P1>$C1"
  elif (( $P1 == $C1 )) ; then
    if (( $P2 > $C2 )) ; then
      DOUPDATE=1
      echo "P2>C2 $P2>$C2"
    fi
  fi
fi

if (( $DOUPDATE == 1 )) ; then
  echo "Start firmware update"
  /usr/local/robotino/daemons/bin/controld3 -enterBootloader
else
  echo "Firmware up to date"
fi
