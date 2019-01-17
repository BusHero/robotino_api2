#!/bin/bash
DEVICE=/dev/robotino3io_bootloader
LOGFILE=@LOGFILE@
FIRMWARE=/usr/local/robotino/firmware/firmware-@VERSION_STRING@.bin
VERSION=@VERSION_STRING@

if [ -n "$LOGFILE" ] ; then
  echo "**************************" >> $LOGFILE
fi

mylog()
{
  echo "$1"
  if [ -n "$LOGFILE" ] ; then
    echo "$1" >> $LOGFILE
  fi  
}

errorexit()
{
  mylog "ERROR: $1"
  exit 1
}

mylog "Installing firmware version $VERSION"

if [ -e $DEVICE ]; then
    for i in `seq 1 10`;
    do
		echo Waiting $i
		sleep 1
	done

    mylog "Try clearing $DEVICE"
	if [ -n "$LOGFILE" ] ; then
		mdel -i $DEVICE ::*.* 1>>$LOGFILE 2>>$LOGFILE
	else
		mdel -i $DEVICE ::*.*
	fi
	
	if [ $? -eq 0 ] ; then
		mylog "Success clearing $DEVICE"
	else
		errorexit "Failed clearing $DEVICE"
	fi
  
    for i in `seq 1 10`;
    do
		echo Waiting $i
		sleep 1
	done
	
	mylog "Try copying firmware to $DEVICE"
	if [ -n "$LOGFILE" ] ; then
		mcopy -i $DEVICE $FIRMWARE ::firmware.bin 1>>$LOGFILE 2>>$LOGFILE
	else
		mcopy -i $DEVICE $FIRMWARE ::firmware.bin
	fi
	
	if [ $? -eq 0 ] ; then
	  mylog "Success installing firmware version $VERSION"
	  exit 0
	fi
else
  mylog $DEVICE does not exist
fi

mylog "Failed installing firmware version $VERSION"
exit 1


