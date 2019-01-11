#!/bin/bash

# script  will set the approbiate settings for Client/AP Mode
# to show wget outputs delete parameter  -q    or add parameter -d for even more debug-infos

SKIPIFCONFIG=0
if [ "$1" == "skip" ] ; then
	SKIPIFCONFIG=1
fi

SCRIPT=$(readlink -f $0)
SCRIPTPATH=`dirname $SCRIPT`

LOGFILE=/var/log/lcdd_scripts
AP_SSID_FILE=/etc/robotino/allnet/AP_SSID

CL_SSID=""
CL_SSID_FILE=/etc/robotino/allnet/CLIENT_SSID

if [ -e "$CL_SSID_FILE" ] ; then
	CL_SSID=`cat "$CL_SSID_FILE"`
fi

if [ -z "$CL_SSID" ] ; then
	CL_SSID=RobotinoAPX.1
fi

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

AP_SSID=""
if [ -e "$AP_SSID_FILE" ] ; then
	AP_SSID=`cat $AP_SSID_FILE`
fi
if [ -z "$AP_SSID" ] ; then
	myecho "Reading SSID"
	$SCRIPTPATH/allnet_getssid.sh skip
	if [ 0 -eq $? ] ; then
		AP_SSID=`cat /tmp/allnet_ssid.txt`
		mkdir -p /etc/robotino/allnet
		echo $AP_SSID > $AP_SSID_FILE
	else
		myexit 1
	fi
fi

myecho "Searching..."
wget http://${ALLNET_IP}/status.asp $WGETPARAM -O /tmp/.wget
RET=$?
if [ 0 -ne $RET ] ; then
	wget http://${ALLNET_IP}/status.asp $WGETPARAM -O /tmp/.wget || myexit 1 "Not found"
fi

grep "wlanMode\[0] =1;" /tmp/.wget > /dev/null
if [ 0 -ne $? ] ; then
	echo "Switch to client"
	myexit 1 "mode first"
fi

myecho "Setup SSID..."
wget http://$ALLNET_IP/goform/formWlanSetup --post-data="ssid0=$CL_SSID&wlan-url=%2Fwlbasic.asp&save=Apply+Changes&basicrates0=15&operrates0=4095"  $WGETPARAM -O-  1>/dev/null || myexit 1 "Failed"
RET=$?
if [ 0 -ne $RET ] ; then
	wget http://$ALLNET_IP/goform/formWlanSetup --post-data="ssid0=$CL_SSID&wlan-url=%2Fwlbasic.asp&save=Apply+Changes&basicrates0=15&operrates0=4095"  $WGETPARAM -O-  1>/dev/null || myexit 1 "Failed"
fi


myecho "Setup DHCP..."
wget http://$ALLNET_IP/goform/formTcpipSetup    --post-data="dhcp=0&save=Apply+Changes&submit-url=%2Ftcpip.asp"  $WGETPARAM  -O- 1>/dev/null
RET=$?
if [ 0 -ne $RET ] ; then
	wget http://$ALLNET_IP/goform/formTcpipSetup    --post-data="dhcp=0&save=Apply+Changes&submit-url=%2Ftcpip.asp"  $WGETPARAM  -O- || myexit 1 "Failed"
fi

for i in {1..30}
do
	myecho "wait $i/30"
	sleep 1
done

myecho "Check..."
wget http://$ALLNET_IP/status.asp $WGETPARAM -O- | grep "^ssid_drv\[0\] ='$CL_SSID';" > /dev/null
RET=$?
if [ 0 -ne $RET ] ; then
	wget http://$ALLNET_IP/status.asp $WGETPARAM -O- | grep "^ssid_drv\[0\] ='$CL_SSID';" > /dev/null
	if [ 0 -ne $RET ] ; then
		echo "Failed writing SSID"
		myexit 1 "$CL_SSID"
	fi
fi

$SCRIPTPATH/allnet_find.sh skip

myexit 0

