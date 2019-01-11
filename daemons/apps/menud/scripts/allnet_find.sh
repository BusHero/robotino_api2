#!/bin/bash

# returns 0 if Allnet AP with factory settings is found
# returns 1 if Allnet AP with Festo settings in Client mode is found
# returns 2 if Allnet AP with Festo settings in AP mode is found
# returns 3 if no Allnet AP is found

SKIPIFCONFIG=0
if [ "$1" == "skip" ] ; then
	SKIPIFCONFIG=1
fi

SCRIPT=$(readlink -f $0)
SCRIPTPATH=`dirname $SCRIPT`

LOGFILE=/var/log/lcdd_scripts
OUTFILE=/tmp/allnet_mode.txt

rm -f $OUTFILE
rm -f /tmp/allnet_ssid.txt

ALLNET_IP1=192.168.1.1
ALLNET_PW1=admin
ALLNET_IP2=172.26.101.1
ALLNET_PW2=apap6

WGETPARAM="--auth-no-challenge  --timeout=8 --tries=2 --http-user=admin  --wait=2 -q"   #-q  #-d
WGETPARAM1="$WGETPARAM --http-passwd=$ALLNET_PW1"
WGETPARAM2="$WGETPARAM --http-passwd=$ALLNET_PW2"

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

myecho "Searching..."
if [ 0 -eq $SKIPIFCONFIG ] ; then
	ifconfig eth0:10 172.26.101.103 netmask 255.255.0.0 up
	ping -c 2 $ALLNET_IP2 1>/dev/null 2>/dev/null
fi
wget http://$ALLNET_IP2/status.asp $WGETPARAM2 -O /tmp/.wget
RET=$?
if [ 0 -ne $RET ] ; then
	wget http://$ALLNET_IP2/status.asp $WGETPARAM2 -O /tmp/.wget
	RET=$?
fi

if [ 0 -eq $RET ] ; then
	myecho "Festo settings"
	grep "wlanMode\[0] =0;" /tmp/.wget > /dev/null
	if [ 0 -eq $? ] ; then
	    echo "AP" > "$OUTFILE"
		$SCRIPTPATH/allnet_getssid.sh skip
		myexit 0 "AP mode"
	fi
    echo "CL" > "$OUTFILE"
	$SCRIPTPATH/allnet_getssid.sh skip
	myexit 0 "Client mode"
fi

ifconfig eth0:10 192.168.1.250 netmask 255.255.255.0 up
ping -c 2 $ALLNET_IP1 1>/dev/null 2>/dev/null
myecho "Searching..."
wget http://${ALLNET_IP1}/status.asp $WGETPARAM1 -O- > /dev/null
RET=$?
if [ 0 -ne $RET ] ; then
	wget http://${ALLNET_IP1}/status.asp $WGETPARAM1 -O- > /dev/null
	RET=$?
fi
if [ 0 -ne $RET ] ; then
	myecho "Searching..."
	wget http://${ALLNET_IP1}/status.asp $WGETPARAM2 -O- > /dev/null
	RET=$?
fi
if [ 0 -ne $RET ] ; then
	wget http://${ALLNET_IP1}/status.asp $WGETPARAM2 -O- > /dev/null
	RET=$?
fi

if [ 0 -eq $RET ] ; then
	echo "Factory" > "$OUTFILE"
	myexit 0 "Factory settings"
fi


echo "NA" > "$OUTFILE"
myexit 3 "No Allnet AP found"
