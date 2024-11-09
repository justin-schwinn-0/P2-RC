#!/bin/bash


# Change this to your netid
netid=jts200006

# Your main project name 
PROG=P2-RC

CONFIGNAME="asyncConfig.txt"
# Root directory of your project
PROJDIR=/home/justin/Desktop/code/currentProj/
# Directory where the config file is located on your local system
CONFIGLOCAL=$PROJDIR/${CONFIGNAME}

CONFIGDOCKER=/home/010/j/jt/jts200006/cs6378/${PROG}/${CONFIGNAME}

n=0

cat $CONFIGDOCKER | sed -e "s/#.*//" | sed -e "/^\s*$/d" |
(
    read i
    ii=$( echo $i| awk '{ print $1 }' )
    echo Hosts: $ii
    while [[ $n -lt $ii ]]
    do
    	read line
        host=$( echo $line | awk '{ print $2 }' )

        echo $host
        gnome-terminal -- ssh $netid@$host "killall -u ${netid}" &

        n=$(( n + 1 ))
    done
   
)

echo "Cleanup complete"
