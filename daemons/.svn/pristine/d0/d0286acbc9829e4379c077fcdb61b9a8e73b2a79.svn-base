#!/bin/bash

CONFIG=$1

if [ -z "$CONFIG" ] ; then
  CONFIG="/etc/wpa_supplicant/wpa_supplicant.conf"
fi

echo Using config
echo "$CONFIG"

wpa_supplicant -i wlan0 -D wext -c "$CONFIG" -B
