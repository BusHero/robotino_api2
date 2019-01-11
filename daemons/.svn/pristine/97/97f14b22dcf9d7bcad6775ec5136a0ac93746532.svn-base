#!/bin/bash

SCRIPT=$(readlink -f $0)
SCRIPTPATH=`dirname $SCRIPT`

$SCRIPTPATH/mod_wpa_supplicant.pl $@ || exit 1;
$SCRIPTPATH/stop_wpa_supplicant.sh
$SCRIPTPATH/start_wpa_supplicant.sh || exit 1;
