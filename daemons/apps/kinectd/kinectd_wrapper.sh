#!/bin/bash

for ((;;))
do
  /usr/local/robotino/daemons/bin/kinectd $@
  if [ 0 -eq $? ] ; then
    exit
  fi
  valid=`lsusb | grep "045e:02ae"`
  if [ "" == "$valid" ] ; then
    echo "No kinect available"
    exit
  fi
  sleep 1
done
