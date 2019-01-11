#!/bin/bash

PREV_IMAGE=$1

if [ "$PREV_IMAGE" == "" ] ; then
  PREV_IMAGE="unknown"
fi

CODENAME=`lsb_release -a 2>/dev/null | grep Description | gawk '{$1 = ""; print $0}'`
CODENAME=`echo "${CODENAME:1:${#CODENAME}-1}"`

PACKAGES=(rec-rpc
robotino-api2
robotino-common
robotino-config
robotino-tools
robotino-daemons
robotino-examples
robotino3-firmware
robview3
robotino-smartsoft-common
robotino-smartsoft-master
robotino-smartsoft-slave
robotino-smartsoft-src
)

IMAGEVER=`cat /version.txt`

if [ "$IMAGEVER" == "" ] ; then
  IMAGEVER="unknown"
fi

DATABASE=/dev/stdout

function keyValueStr()
{
  echo "\"$1\":\"$2\""
}

function writekeyValuePair()
{
  str=$(keyValueStr "$1" "$2")
  echo "${str}$3" >> $DATABASE
}

echo "{"  > $DATABASE
writekeyValuePair "OS" "$CODENAME" ","
writekeyValuePair "current" "$IMAGEVER" ","
writekeyValuePair "prev" "$PREV_IMAGE" ","

echo "\"packages\":{" >> $DATABASE

PACKAGES_LEN=${#PACKAGES[@]}

PAIRS=()

for (( i=0; i<${PACKAGES_LEN}; i++ ));
do
  pkg=${PACKAGES[$i]}
	
  ver=`dpkg -s $pkg 2>/dev/null | grep Version | gawk '{print $2}'`
  
  if [ "$ver" != "" ] ; then
	str=$(keyValueStr $pkg $ver)
	
	PAIRS+=($str)
  fi
done

PAIRS_LEN=${#PAIRS[@]}
for (( i=0; i<${PAIRS_LEN}; i++ ));
do
  str=${PAIRS[$i]}
  
  endchar=""
  if (( i+1 < ${PAIRS_LEN} )); then
    endchar=","
  fi
  
  echo $str$endchar >> $DATABASE
done

echo "}"  > $DATABASE
echo "}"  > $DATABASE
