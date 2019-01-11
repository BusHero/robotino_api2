#!/bin/sh

#exit codes:
#0: Unkown cpu
#1: VIA Eden processor
#2: AMD Geode processor

grep "VIA Samuel 2" /proc/cpuinfo
if [ 0 -eq $? ] ; then
  exit 1;
fi

grep "Geode(TM) Integrated Processor by AMD PCS" /proc/cpuinfo
if [ 0 -eq $? ] ; then
  exit 2;
fi

exit 0;