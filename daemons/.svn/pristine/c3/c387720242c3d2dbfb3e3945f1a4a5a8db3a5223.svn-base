#!/bin/bash

SKIPIFCONFIG=0
if [ "$1" == "skip" ] ; then
	SKIPIFCONFIG=1
fi

LOGFILE=/var/log/lcdd_scripts
OUTFILE=/tmp/allnet_channel.txt

ALLNET_IP=172.26.101.1
WGETPARAM="--auth-no-challenge  --timeout=8 --tries=2 --http-user=admin --http-passwd=apap6 --wait=2 -q"   #-q  #-d

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

wget http://${ALLNET_IP}/status.asp $WGETPARAM   -O /tmp/.wget
RET=$?
if [ 0 -ne $RET ] ; then
	wget http://${ALLNET_IP}/status.asp $WGETPARAM   -O /tmp/.wget || myexit 1 "wget error"
	RET=$?
fi

a=`grep "^channel_drv\[0\] =\(.*\);" /tmp/.wget | sed -n "s/channel_drv\[0] =//;s/\;//;s/'//g;p"` || myexit 2 "grep channel error"
b=`grep "^state_drv\[0\] =\(.*\);" /tmp/.wget | sed -n "s/state_drv\[0] =//;s/\;//;s/'//g;p"` || myexit 2 "grep state error"
if [ "$b" = "Scanning" ] 
then 
   a="$a?"   
fi
echo $a > $OUTFILE
myexit 0 $a