#!/bin/bash

DAEMONSLIST=$1

function list_include_item {
  local list="$1"
  local item="$2"
  if [[ $list =~ (^|[[:space:]])"$item"($|[[:space:]]) ]] ; then
    # yes, list include item
    result=0
  else
    result=1
  fi
  return $result
}

if list_include_item "$DAEMONSLIST" "smartsoft-common" ; then
  echo smartsoft-common included
  DAEMONSLIST=$(echo $DAEMONSLIST | sed -e "s/ *smartsoft-master */ /"); 
  DAEMONSLIST=$(echo $DAEMONSLIST | sed -e "s/ *smartsoft-slave */ /"); 
fi

if list_include_item "$DAEMONSLIST" "smartsoft-master" ; then
  echo smartsoft-master included
  DAEMONSLIST=$(echo $DAEMONSLIST | sed -e "s/ *smartsoft-slave */ /"); 
fi

for d in $DAEMONSLIST
do
  if [ "smartsoft-common" == "$d" ] ; then
    echo systemctl stop smartsoft-master
    echo systemctl stop smartsoft-slave
  fi
done