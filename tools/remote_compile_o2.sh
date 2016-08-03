#!/bin/sh

## define the remote server 
pkg="wdb40"
server="lazar@build.onion.io"
remotePath="/home/lazar/OpenWRT-Buildroot-Omega2/openwrt/dl"

## upload project to openwrt2.onion.io 
localPath="../$pkg"


cmd="rsync -va --progress $localPath $server:$remotePath"
echo "$cmd"
eval "$cmd"


## create a tar from the file, run the compile
cmd="ssh $server \"cd $remotePath && tar -zcvf $pkg.tar.gz $pkg && cd .. && make package/feeds/onion/$pkg/compile V=99\""
echo "$cmd"
eval "$cmd"

