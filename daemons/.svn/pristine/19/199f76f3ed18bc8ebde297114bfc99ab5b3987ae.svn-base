#!/bin/bash

# call script  ./allnet_ap_init.sh 5.123  to programm  SSID Robotino.5.123
# to show wget outputs delete parameter  -q    or add parameter -d for even more debug-infos

LOGFILE=/var/log/lcdd_scripts

if [ "" == "$1" ] ; then
	n1=$RANDOM
	n1=${n1:0:3}
	n2=$RANDOM
	n2=${n2:0:3}
	AP_SSID="Robotino.$n1.$n2"
else
	AP_SSID=Robotino.$1
fi

SCRIPT=$(readlink -f $0)
SCRIPTPATH=`dirname $SCRIPT`

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
	ifconfig eth0:10 down 2>/dev/null
	if [ "$2" != "" ] ; then
		myecho "$2"
	fi
	exit "$1"
}

ifconfig eth0:10 192.168.1.250 netmask 255.255.255.0 up
ping -c 2 $ALLNET_IP1 1>/dev/null 2>/dev/null

ALLNETIP="$ALLNET_IP1"
WGETPARAM="$WGETPARAM1"

myecho "Searching..."
wget http://${ALLNETIP}/status.asp $WGETPARAM -O /tmp/.wget
RET=$?
if [ 0 -ne $RET ] ; then
	wget http://${ALLNETIP}/status.asp $WGETPARAM -O /tmp/.wget
	RET=$?
fi
if [ 0 -ne $RET ] ; then
	WGETPARAM="$WGETPARAM2"
	myecho "Searching..."
	wget http://${ALLNETIP}/status.asp $WGETPARAM -O /tmp/.wget
	RET=$?
fi
if [ 0 -ne $RET ] ; then
	wget http://${ALLNETIP}/status.asp $WGETPARAM -O /tmp/.wget
	RET=$?
fi

if [ 0 -ne $RET ] ; then
	ALLNETIP="$ALLNET_IP2"
	WGETPARAM="$WGETPARAM2"
	ifconfig eth0:10 172.26.101.103 netmask 255.255.0.0 up
	ping -c 2 $ALLNETIP 1>/dev/null 2>/dev/null
	myecho "Searching..."
	wget http://${ALLNETIP}/status.asp $WGETPARAM -O /tmp/.wget
	RET=$?
	if [ 0 -ne $RET ] ; then
		wget http://${ALLNETIP}/status.asp $WGETPARAM -O /tmp/.wget
		RET=$?
	fi
	if [ 0 -ne $RET ] ; then
		WGETPARAM="$WGETPARAM1"
		myecho "Searching..."
		wget http://${ALLNETIP}/status.asp $WGETPARAM -O /tmp/.wget
		RET=$?
	fi
	if [ 0 -ne $RET ] ; then
		wget http://${ALLNETIP}/status.asp $WGETPARAM -O /tmp/.wget
		RET=$?
	fi
fi

if [ 0 -ne $RET ] ; then
	myexit 1 failed
fi

#grep "^ssid_drv\[0\] ='WLAN-11g-AP';" /tmp/.wget || error_exit "No Allnet AP with factory settings found"

#myecho "Allnet AP with factory settings found"
#myecho

grep "wlanMode\[0] =0;" /tmp/.wget > /dev/null || myexit 1 "Not in AP mode"

myecho "Reset..."
wget http://$ALLNETIP/goform/formSaveConfig --post-data="reset=Reset"  $WGETPARAM -O- 1>/dev/null
RET=$?
if [ 0 -ne $RET ] ; then
	wget http://$ALLNETIP/goform/formSaveConfig --post-data="reset=Reset"  $WGETPARAM -O- 1>/dev/null || myexit 1 "Failed"
fi

for i in {1..30}
do
	myecho "wait $i/30"
	sleep 1
done

ifconfig eth0:10 192.168.1.250 netmask 255.255.255.0 up
ping -c 2 $ALLNETIP 1>/dev/null 2>/dev/null

myecho "Check..."
wget http://$ALLNET_IP1/status.asp $WGETPARAM1 -O- | grep "^ssid_drv\[0\] ='WLAN-11g-AP';" 1>/dev/null
RET=$?
if [ 0 -ne $RET ] ; then
	wget http://$ALLNET_IP1/status.asp $WGETPARAM1 -O- | grep "^ssid_drv\[0\] ='WLAN-11g-AP';" || myexit 1 "Failed"
fi

$SCRIPTPATH/allnet_find.sh skip

myexit 0

