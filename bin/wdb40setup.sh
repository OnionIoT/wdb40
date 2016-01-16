#!/bin/sh

## script to setup uci wireless configuration for use with wdb40 utility

### global variables
# options
bVerbose=0

#commands
bCmdAdd=0
bCmdRemove=0

#parameters
bApNetwork=0


########################
##### Functions #####
# find total number of configured wifi networks
# 	returns value via echo
_FindNumNetworks () {
	local count=0

	# find the first network
	local network=$(uci -q get wireless.\@wifi-iface[$count])

	# loop through all configured networks
	while [ "$network" == "wifi-iface" ]
	do
		# add to the count
		count=$(($count + 1))

		# continue the loop
		network=$(uci -q get wireless.\@wifi-iface[$count])
	done

	# return the count number
	echo $count
}

# find a network's array number by the ssid
#	$1	- ssid to look for
#	returns value via echo
#		-1: 		if not found
#		all others: valid value found
_FindNetworkBySsid () {
	local id=-1
	local count=0
	local ssidKey="$1"

	# ensure argument is present
	if [ "$ssidKey" != "" ]; then

		# find the first network
		local network=$(uci -q get wireless.\@wifi-iface[$count])

		# loop through all configured networks
		while [ "$network" == "wifi-iface" ]
		do
			# find the ssid
			local ssid=$(uci -q get wireless.\@wifi-iface[$count].ssid)

			if [ "$ssid" == "$ssidKey" ]; then
				id=$count
				break
			fi

			# continue the loop
			count=$(($count + 1))
			network=$(uci -q get wireless.\@wifi-iface[$count])
		done
	fi

	# return the count number
	echo $id
}

# find the AP network's array number
#	returns value via echo
#		-1: 		if not found
#		all others: valid value found
_FindApNetwork () {
	local id=-1
	local count=0

	# find the first network
	local network=$(uci -q get wireless.\@wifi-iface[$count])

	# loop through all configured networks
	while [ "$network" == "wifi-iface" ]
	do
		# find the ssid
		local mode=$(uci -q get wireless.\@wifi-iface[$count].mode)

		if [ "$mode" == "ap" ]; then
			id=$count
			break
		fi

		# continue the loop
		count=$(($count + 1))
		network=$(uci -q get wireless.\@wifi-iface[$count])
	done

	# return the count number
	echo $id
}

# Add/edit a uci section for a wifi network
#	$1 	- interface number
#	$2 	- interface type "ap" or "sta"
_AddWifiUciSection () {
	local commit=1
	local id=$1
	local networkType=$2
	local bNew=0
	

	# setup new intf if required
	local iface=$(uci -q get wireless.\@wifi-iface[$id])
	if [ "$iface" != "wifi-iface" ]; then
		uci add wireless wifi-iface > /dev/null
		uci set wireless.@wifi-iface[$id].device="radio0" 
		bNew=1
	fi

	# perform the type specific setup
	if [ "$networkType" = "sta" ]; then
		if [ $bNew == 1 ]; then
			echo "> Adding '$ssid' network to database (priority: $id) "
		else
			echo "> Editing '$ssid' network (priority: $id)"
		fi
		# use UCI to set the network to client mode and wwan
		uci set wireless.@wifi-iface[$id].mode="sta"
		uci set wireless.@wifi-iface[$id].network="wwan"
	elif [ "$networkType" = "ap" ]; then
		echo "> Setting up $ssid Access Point as network $id"
		# use UCI to set the network to access-point mode and wlan
		uci set wireless.@wifi-iface[$id].mode="ap"
		uci set wireless.@wifi-iface[$id].network="wlan"
	fi 

	# use UCI to set the ssid, encryption, and disabled options
	uci set wireless.@wifi-iface[$id].ssid="$ssid"
	uci set wireless.@wifi-iface[$id].encryption="$auth"
	uci set wireless.@wifi-iface[$id].disabled="1"

	# set the network key based on the authentication
	case "$auth" in
		psk|psk2)
			uci set wireless.@wifi-iface[$id].key="$password"
	    ;;
	    wep)
			uci set wireless.@wifi-iface[$id].key=1
			uci set wireless.@wifi-iface[$id].key1="$password"
	    ;;
	    none)
			# set no keys for open networks, delete any existing ones
			local key=$(uci -q get wireless.\@wifi-iface[$id].key)

			if [ "$key" != "" ]; then
				uci delete wireless.@wifi-iface[$id].key
			fi
	    ;;
	    *)
			# invalid authorization
			commit=0
	esac


	# commit the changes
	if [ $commit == 1 ]; then
		uci commit wireless
	fi
}

# remove a uci section that defines a wifi network 
#	$1 - iface number
_DeleteWifiUciSection () {
	local commit=1

	# check the argument
	if [ $1 -ge 0 ]; then
		# ensure that iface exists
		local iface=$(uci -q get wireless.\@wifi-iface[$1])
		if [ "$iface" == "wifi-iface" ]; then
			echo "> Removing '$ssid' network from database"
			uci delete wireless.@wifi-iface[$1]
			uci commit wireless
		fi
	fi
}



########################
##### Main Program #####

# parse arguments
while [ "$1" != "" ]
do
	case "$1" in
		# options
		-v|--v|-verbose|verbose)
			bVerbose=1
			shift
		;;
		# commands
    	-add|add|-edit|edit)
			bCmdAdd=1
			shift
		;;
		-remove|remove)
			bCmdRemove=1
			shift
		;;
		# parameters
		-ssid|ssid)
			shift
			ssid="$1"
			shift
		;;
		-password|password)
			shift
			password="$1"
			shift
		;;
		-encr|encr)
			shift
			auth=$1
			shift
		;;
		-priority|priority)
			shift
			priorityId=$1
			shift
		;;
	    *)
			echo "ERROR: Invalid Argument: $1"
			shift
		;;
	esac
done


## parameter processing
if [ $bApNetwork == 1 ]; then
	networkType="ap"
else
	networkType="sta"
fi


## commands
# check if network already exists in configuration
id=$(_FindNetworkBySsid "$ssid")

if [ $bCmdAdd == 1 ]; then
	# if it doesn't already exist, add a new section
	if [ $id == -1 ]; then
		id=$(_FindNumNetworks)
	fi

	# add or edit the uci entry
	_AddWifiUciSection $id $networkType

elif [ $bCmdRemove == 1 ]; then
	# only remove existing networks
	if [ $id != -1 ]; then
		_DeleteWifiUciSection $id
	fi
fi

