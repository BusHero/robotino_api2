#!/bin/bash

SKIPIFCONFIG=0
if [ "$1" == "skip" ] ; then
	SKIPIFCONFIG=1
fi

ALLNET_IP=172.26.101.1
WGETPARAM="--auth-no-challenge  --timeout=8 --tries=2 --http-user=admin --http-passwd=apap6 --wait=2 -q"   #-q  #-d

LOGFILE=/var/log/lcdd_scripts
OUTFILE=/tmp/allnet_ssid.txt

rm -f $OUTFILE

myecho()
{
	echo "$1"
	echo "$1" >> $LOGFILE
}

myexit()
{
	if [ 0 -eq $SKIPIFCONFIG ] ; then
		ifconfig eth0:10 down 2>/dev/null
	fi

	if [ "$2" != "" ] ; then
		myecho "$2"
	fi

	exit "$1"
}

if [ 0 -eq $SKIPIFCONFIG ] ; then
	ifconfig eth0:10 172.26.101.103 netmask 255.255.0.0 up
	ping -c 2 $ALLNET_IP 1>/dev/null 2>/dev/null
fi

SSID=`wget http://${ALLNET_IP}/status.asp $WGETPARAM   -O- | grep "^ssid_drv\[0\] =\(.*\);" | sed -n "s/ssid_drv\[0] =//;s/\;//;s/'//g;p"`
RET=$?
if [ 0 -ne $RET ] ; then
	SSID=`wget http://${ALLNET_IP}/status.asp $WGETPARAM   -O- | grep "^ssid_drv\[0\] =\(.*\);" | sed -n "s/ssid_drv\[0] =//;s/\;//;s/'//g;p"`
	RET=$?
fi

if [ 0 -eq $RET ] ; then
	echo $SSID > $OUTFILE
	myexit 0 "$SSID"
fi

myexit 1 "Not found"