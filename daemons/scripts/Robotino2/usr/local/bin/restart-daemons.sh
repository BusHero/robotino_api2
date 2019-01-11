#!/bin/bash

if [ $1 == "wait" ] ; then
ret=0
while [ $ret -ne 8 ]
do
  /usr/local/robotino/daemons/bin/lcdd2cmd -wait=0 "Press enter" "to restart" services
ret=$?
done
fi

/etc/init.d/lcdd2cmd.sh || true
/etc/init.d/rtai.sh start || true
/etc/init.d/rpcd.sh start || true
/etc/init.d/webd.sh start || true
/etc/init.d/api1d.sh start || true
/etc/init.d/controld2.sh start || true
/etc/init.d/lcdd2.sh start || true
/etc/init.d/menud.sh start || true
/usr/local/robotino/daemons/bin/control3d -daemon || true

