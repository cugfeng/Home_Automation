#!/bin/sh

FLAG="/tmp/temperature/automode"
sudo chmod 0666 $FLAG

status="unknown"
while [ 1 ]; do
	hour=`date +%H`
    if [ $hour -eq 6 ] && [ $status != "disabled" ]; then
        echo "Time is 6:00, disable automode"
        echo 0 > $FLAG
		status="disabled"
		echo "Turn off AC"
		irsend SEND_ONCE AC OFF
	elif [ $hour -eq 22 ] && [ $status != "enabled" ]; then
		echo "Time is 22:00, enable automode"
		echo 1 > $FLAG
		status="enabled"
    fi

	sleep 60
done

