#!/bin/sh

## daemon to facilitate wdb40 Wireless Network Tool

# global variables
PROG="/usr/bin/wdb40"
INITIAL_TIMEOUT="60"

bBoot=0



########################
##### Functions #####

# function to run at boot
BootSequence () {
	# ensure that wireless device is up
	ret=$($PROG -t $INITIAL_TIMEOUT wait)
	parsed=$(echo $ret | grep -i timeout)

	# check for timeout
	if [ "$parsed" == "" ]; then
		# perform init sequence:
		#	enable AP (if any)
		#	disable all STA networks
		ret=$($PROG init)

		# wait for wireless device to be up
		ret=$($PROG wait)
		parsed=$(echo $ret | grep -i timeout)

		# check for timeout
		if [ "$parsed" == "" ]; then
			ret=$($PROG scan)
			ret=$($PROG connect)

			# check that connection is successful
			ret=$($PROG -t 20 waitWwan)
			parsed=$(echo $ret | grep -i timeout)

			# check for timeout
			if [ "$parsed" == "" ]; then
				ret=$($PROG disable)
			fi
		fi
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
	    *)
			echo "ERROR: Invalid Argument: $1"
			shift
		;;
	esac
done




### sequence to run on boot
if [ $bBoot == 1 ]; then
	BootSequence
fi
