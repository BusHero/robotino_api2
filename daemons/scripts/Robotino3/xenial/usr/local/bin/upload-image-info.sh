#!/bin/bash

FTP_USER=$1
FTP_PASSWD=$2

if [ "$#" -lt 2 ] ; then
  echo "Illegal number of parameters"
  exit
fi

TMPFILE=/tmp/__upload_image_info.tmp
rm -f $TMPFILE

while read line
do
  echo "$line" >> $TMPFILE
done < "${3:-/dev/stdin}"

image=`jq -r '.current?' $TMPFILE`

if [ "$image" != "" ] ; then
  wput $TMPFILE ftp://$1:$2@alfa3013.alfahosting-server.de/images/$image.data
else
  echo "Error reading image version"
fi