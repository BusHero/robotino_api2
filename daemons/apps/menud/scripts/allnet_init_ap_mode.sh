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

AP_SSID=""
if [ -e "$AP_SSID_FILE" ] ; then
	AP_SSID=`cat $AP_SSID_FILE`
fi

ALLNET_IP=172.26.101.1
WGETPARAM="--auth-no-challenge  --timeout=8 --tries=2 --http-user=admin --http-passwd=apap6 --wait=2 -q"   #-q  #-d

if [ 0 -eq $SKIPIFCONFIG ] ; then
	ifconfig eth0:10 172.26.101.103 netmask 255.255.0.0 up
	ping -c 2 $ALLNET_IP 1>/dev/null 2>/dev/null
fi

myecho "Searching..."
wget http://${ALLNET_IP}/status.asp $WGETPARAM -O /tmp/.wget
RET=$?
if [ 0 -ne $RET ] ; then
	wget http://${ALLNET_IP}/status.asp $WGETPARAM -O /tmp/.wget || myexit 1 "Not found"
fi

grep "wlanMode\[0] =0;" /tmp/.wget > /dev/null
if [ 0 -ne $? ] ; then
	echo "Switch to AP"
	myexit 1 "mode first"
fi

if [ "$AP_SSID" != "" ] ; then
	myecho "Setup SSID..."
	wget http://$ALLNET_IP/goform/formWlanSetup --post-data="ssid0=$AP_SSID&wlan-url=%2Fwlbasic.asp&save=Apply+Changes&basicrates0=15&operrates0=4095" $WGETPARAM -O- 1>/dev/null  || myexit 1 "Setup new SSID in AP mode"
fi

myecho "Setup DHCP..."
wget http://$ALLNET_IP/goform/formTcpipSetup --post-data="dhcp=2&save=Apply+Changes&submit-url=%2Ftcpip.asp" $WGETPARAM -O- 1>/dev/null || myexit 1 "Setup DHCP=ON"

for i in {1..30}
do
	myecho "wait $i/30"
	sleep 1
done

if [ "$AP_SSID" != "" ] ; then
	myecho "Check..."
	wget http://$ALLNET_IP/status.asp $WGETPARAM -O- | grep "^ssid_drv\[0\] ='$AP_SSID';" > /dev/null
	RET=$?
	if [ 0 -ne $RET ] ; then
		wget http://$ALLNET_IP/status.asp $WGETPARAM -O- | grep "^ssid_drv\[0\] ='$AP_SSID';" > /dev/null
		if [ 0 -ne $RET ] ; then
			echo "Failed writing SSID"
			myexit 1 "$AP_SSID"
		fi
	fi
fi

$SCRIPTPATH/allnet_find.sh skip

myexit 0

