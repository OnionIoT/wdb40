#!/bin/sh

## define the remote server 
server="lazar@openwrt2.onion.io"
remotePath="/home/lazar/OpenWRT-Buildroot/openwrt/dl"

## upload project to openwrt2.onion.io 
localPath="../wdb40"


cmd="rsync -va --progress $localPath $server:$remotePath"
echo "$cmd"
eval "$cmd"


## create a tar from the file, run the compile
cmd="ssh $server \"cd $remotePath && tar -zcvf wdb40.tar.gz wdb40 && cd .. && make package/feeds/onion/wdb40/compile V=99\""
echo "$cmd"
eval "$cmd"

