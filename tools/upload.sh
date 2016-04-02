#!/bin/sh

# check for argument
if [ "$1" == "" ]
then
    echo "ERROR: expecting Omega hex code as argument!"
    echo "$0 <hex code>"
    exit
fi


##
localPath="bin/wdb40.sh"
remotePath="/usr/bin/wdb40"

cmd="rsync -va --progress $localPath root@omega-$1.local:$remotePath"
echo "$cmd"
eval "$cmd"


##
localPath="bin/wdb40setup.sh"
remotePath="/usr/bin/wifisetup"

cmd="rsync -va --progress $localPath root@omega-$1.local:$remotePath"
echo "$cmd"
eval "$cmd"


##
localPath="init.d/wdb40d.sh"
remotePath="/etc/init.d/wdb40d"

cmd="rsync -va --progress $localPath root@omega-$1.local:$remotePath"
echo "$cmd"
eval "$cmd"

