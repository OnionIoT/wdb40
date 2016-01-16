#!/bin/sh

## daemon to facilitate wdb40 Wireless Network Tool

# global variables
PROG="/usr/bin/wdb40tool"
INITIAL_TIMEOUT="60"
CONNECTION_TIMEOUT="20"
SHORT_TIMEOUT="2"

bBoot=0
bVerbose=0



########################
##### Functions #####
# function to print arg1 if bVerbose is enabled
_Print () {
	if [ $bVerbose == 1 ]; then
		echo $1
	fi
}

# function to connect to the first network in the match networks file
#	enables configured network that was in the scan
#	checks that connection was successful (wwan interface is up)
#		if not successful:
#			enable the AP network
#			disable all STA networks
# arguments:
# 	arg1 - if set to force, will enable connect force option (wifi setup must be triggered)
_ConnectToMatch() {
	local OPT=""
	local TIMEOUT="$CONNECTION_TIMEOUT"

	# check for arguments
	if [ "$1" != "" ]; then
		if [ "$1" == "force" ]; then
			OPT="-f"
		fi
	fi

	# connect to first network in the match file
	local ret=$($PROG $OPT connect)
	_Print "-> CONNECT: $ret"
	# check to see there was a connection attempt
	local noMatchFound=$(echo $ret | grep "Scan found no matching networks")

	# if there was no connection attempt, shorten the timeout
	if [ "$noMatchFound" != "" ]; then
		TIMEOUT="$SHORT_TIMEOUT"
	fi

	# check that connection is successful
	ret=$($PROG -t $TIMEOUT waitWwan)
	_Print "-> WAITWWAN: $ret"
	local parsed=$(echo $ret | grep -i timeout)

	# check for timeout - indicating incorrect key used for network
	if [ "$parsed" != "" ]; then
		# disable any STA networks, enable the AP
		ret=$($PROG disable)
		_Print "-> DISABLE: $ret"

		# try the next match 
		#	assuming that this connection attempt was to an actual network
		if [ "$noMatchFound" == "" ]; then
			_Print "-> Attempting to connect to next matching network"
			_ConnectToMatch
		fi
	fi
}


# function to run at boot
#	waits until wireless device is up
#	disables all STA networks
#	scans for available wifi networks
#	attempts to connect to any configured networks that show up in the scan
BootSequence () {
	# ensure that wireless device is up
	ret=$($PROG -t $INITIAL_TIMEOUT wait)
	_Print "-> WAIT: $ret"
	parsed=$(echo $ret | grep -i timeout)

	# check for timeout
	if [ "$parsed" == "" ]; then
		# perform init sequence:
		# 	read configured networks
		#	enable AP (if any)
		#	disable all STA networks
		ret=$($PROG init)
		_Print "-> INIT: $ret"

		# wait for wireless device to be up
		ret=$($PROG wait)
		_Print "-> WAIT: $ret"
		parsed=$(echo $ret | grep -i timeout)

		# check for timeout
		if [ "$parsed" == "" ]; then
			# scan for available networks
			ret=$($PROG scan)
			_Print "-> SCAN: $ret"
			
			# connect to configured network that showed up in scan
			_ConnectToMatch "force"
		fi
	fi
}

# function to do another wdb40 connection attempt
# 	check that wireless device is up
# 	scan for available wifi networks
#	attempt to connect to any configured network that shows up in the scan
RegularSequence () {
	# ensure that wireless device is up (regular 10sec timeout)
	ret=$($PROG wait)
	_Print "-> WAIT: $ret"
	parsed=$(echo $ret | grep -i timeout)

	# check for timeout
	if [ "$parsed" == "" ]; then
		# read configured networks
		ret=$($PROG read)
		_Print "-> READ: $ret"

		# scan for available networks
		ret=$($PROG scan)
		_Print "-> SCAN: $ret"
		
		# connect to configured network that showed up in scan
		_ConnectToMatch
	fi
}



########################
##### Main Program #####

# parse arguments
while [ "$1" != "" ]
do
	case "$1" in
    	-boot|boot)
			bBoot=1
			shift
		;;
		-v|--v|-verbose|verbose)
			bVerbose=1
			shift
		;;
	    *)
			echo "ERROR: Invalid Argument: $1"
			shift
		;;
	esac
done




### sequence to run on boot
if [ $bBoot == 1 ]; then
	BootSequence
else
	### regular sequence to run 
	RegularSequence
fi


