#!/bin/sh
case "$1" in
  start)
    echo "Starting aesdsocket"
    start-stop-daemon --start --background --exec /usr/bin/aesdsocket -- -d
    ;;
  stop)
    echo "Stopping aesdsocket"
    start-stop-daemon --stop --exec /usr/bin/aesdsocket
    ;;
  *)
    echo "Usage: /etc/init.d/aesdsocket {start|stop}"
    exit 1
    ;;
esac

exit 0

