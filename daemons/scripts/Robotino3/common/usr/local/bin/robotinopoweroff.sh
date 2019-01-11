#!/bin/bash

pgrep SmartSoft*
if [ 0 -ne $? ] ; then
	/sbin/poweroff
fi
