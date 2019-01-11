#!/bin/sh

SERVICEPATH=/usr/local/robotino/daemons/bin
SERVICE=controld3
PARAM=

startservice()
{
  start-stop-daemon --background --start --quiet --exec $SERVICEPATH/$SERVICE -- $PARAM
  if [ $? -eq 0 ] ; then
    echo $SERVICE started
  fi
}

stopservice()
{
 start-stop-daemon --stop --signal INT --name $SERVICE
 if [ $? -eq 0 ] ; then
  echo $SERVICE stopped
 fi
}

case "$1" in
    start)
        startservice 
        ;;

    stop)
        stopservice 
        ;;

    restart)
        stopservice 
        startservice 
        ;;
    *)
        echo "Usage: $0 {start|stop|restart}"
        exit 1
        ;;
esac

