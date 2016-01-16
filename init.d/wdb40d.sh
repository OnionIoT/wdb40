#!/bin/sh /etc/rc.common
# Copyright (C) 2016 Onion Corporation
START=79

BIN=wdb40

boot() {
	echo "[wdb40d] boot"
	$BIN boot
}


