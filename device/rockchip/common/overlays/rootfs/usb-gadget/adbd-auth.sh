#!/bin/sh -e

for i in $(seq 1 3); do
	echo -n "$(hostname -s)'s password: "
	read -s PASSWD
	if [ "$(echo $PASSWD | md5sum)" = "ADBD_PASSWORD_MD5" ]; then
		echo "success."
		exit 0
	fi

	echo "password incorrect!"
done

false
