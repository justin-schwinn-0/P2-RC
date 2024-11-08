#!/bin/bash

# Change this to your netid
netid=jts200006

# Your main project name 
PROG=P2-RC

CONFIGNAME="${1}"

# Root directory of your project on the remote location
PROJDIR=/home/010/j/jt/jts200006/cs6378/${PROG}

# Directory where the config file is located on your local system
CONFIGLOCAL=/home/justin/Desktop/code/currentProj/${CONFIGNAME}

CONFIGDOCKER=/home/010/j/jt/jts200006/cs6378/${PROG}/${CONFIGNAME}

# Directory your java classes are in
BINDIR=$PROJDIR/build


n=0

cat $CONFIGLOCAL | sed -e "s/#.*//" | sed -e "/^\s*$/d" |
(
    read i
    ii=$( echo $i| awk '{ print $1 }' )
    echo Hosts: $ii
    while [[ $n -lt $ii ]]
    do
    	read line
    	p=$( echo $line | awk '{ print $1 }' )
        host=$( echo $line | awk '{ print $2 }' )
	
	    gnome-terminal -- ssh $netid@$host "cd ${BINDIR}/..;/${BINDIR}/${PROG} ${PROJDIR}/${CONFIGNAME} ${p};" "exec bash" &

        n=$(( n + 1 ))
    done
)
