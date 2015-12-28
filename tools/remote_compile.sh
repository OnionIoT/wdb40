#!/bin/sh

## define the remote server 
server="lazar@openwrt2.onion.io"
remotePath="/home/lazar/OpenWRT-Buildroot/openwrt/dl"

## upload project to openwrt2.onion.io 
localPath="../wdb40"
remoteDir="$remotePath/wdb40"

cmd="rsync -va --progress $localPath $server:$remoteDir"
echo "$cmd"
eval "$cmd"

## create a tar from the file
remoteTar="$remotePath/wdb40.tar.gz"
cmd="ssh $server tar -zcvf $remoteTar $remoteDir"
echo "$cmd"
eval "$cmd"
