#!/bin/sh

## script to setup uci wireless configuration for use with wdb40 utility

. /usr/share/libubox/jshn.sh


### global variables
# options
bVerbose=0

#commands
bCmd=0
bCmdAdd=0
bCmdDisable=0
bCmdRemove=0
bCmdPriority=0


#parameters
bApNetwork=0


#############################
##### General Functions #####
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

# find a networks SSID from the id
#	$1	- network id
_FindNetworkSsid () {
	local name=$(uci -q get wireless.\@wifi-iface[$1].ssid)

	echo $name
}

# Normalize the authentication input
_NormalizeAuthInput () {
	case "$auth" in
		psk2|PSK2|wpa2|WPA2)
			auth="psk2"
		;;
		psk|PSK|wpa|WPA)
			auth="psk"
		;;
		wep|WEP)
			auth="wep"
		;;
		none|*)
			auth="none"
		;;
	esac
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
		none|*)
			# set no keys for open networks, delete any existing ones
			local key=$(uci -q get wireless.\@wifi-iface[$id].key)

			if [ "$key" != "" ]; then
				uci delete wireless.@wifi-iface[$id].key
			fi
		;;
	esac


	# commit the changes
	if [ $commit == 1 ]; then
		uci commit wireless
	fi
}

# disable a uci wifi network section
#	$1 - iface number
_DisableWifiUciSection () {

	# check the argument
	if [ $1 -ge 0 ]; then
		# ensure that iface exists
		local iface=$(uci -q get wireless.\@wifi-iface[$1])
		if [ "$iface" == "wifi-iface" ]; then
			echo "> Disabling '$ssid' network"
			uci set wireless.@wifi-iface[$1].disabled='1'
			uci commit wireless
		fi
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

# reorder a specified uci wifi section
#	$1 	- iface number
# 	$2 	- desired order number in uci config
# 	$3 	- priority number in terms of other networks (optional)
_ReorderWifiUciSection () {
	# check the argument
	if [ $1 -ge 0 ]; then
		# ensure that iface exists
		local iface=$(uci -q get wireless.\@wifi-iface[$1])
		if [ "$iface" == "wifi-iface" ]; then
			# print a message
			if [ "$3" != "" ]; then
				echo "> Shifting '$ssid' priority to $3"
			else
				echo "> Shifting '$ssid' priority"
			fi

			# perform the reorder
			uci reorder wireless.@wifi-iface[$1]=$2
			uci commit wireless
		fi
	fi
}

# change the priority of a network (by changing the uci wireless section order)
#	$1 	- network section id
#	$2 	- argument for moving the network
_SetNetworkPriority () {
	local id=$1
	local argument=$2
	## find the network's current priority
	local currPriority=$(($id + 1))

	## find the top priority
	# find the ap network
	local apId=$(_FindApNetwork)
	local topPriority=0
	
	if [ $apId == -1 ]; then
		# no AP network, top priority spot is 1 (radio0 is spot 0)
		topPriority=1
	else
		# AP network present, top priority spot is 2 (radio0 is spot 0, AP is spot 1)
		topPriority=2
	fi

	## find the lowest priority
	local bottomPriority=$(_FindNumNetworks)


	## find the shift in priority
	if [ "$argument" == "up" ]; then
		desiredPriority=$(($currPriority - 1))
	elif [ "$argument" == "down" ]; then
		desiredPriority=$(($currPriority + 1))
	fi

	## find the new human-readable priority
	local hmnPriority=$(($desiredPriority - $topPriority + 1))

	# check that shift is valid
	if 	[ $desiredPriority -lt $topPriority ] ||
		[ $desiredPriority -gt $bottomPriority ] ||
		[ $currPriority -lt $topPriority ];
	then
		echo "> ERROR: Invalid priority shift requested"
	else
		_ReorderWifiUciSection $id $desiredPriority $hmnPriority
	fi
}



################################
##### User Input Functions #####
# read WPA settings from json data
_UserInputJsonReadNetworkAuthPsk () {
	local bFoundType1=0
	local bFoundType2=0
	local type=""

	# check the wpa object
	json_get_type type wpa

	# read the wpa object
	if [ "$type" == "array" ]
	then
		# select the wpa object
		json_select wpa

		# find all the values
		json_get_values values

		# read all elements
		for value in $values
		do
			# parse value
			if [ $value == 1 ]
			then
				bFoundType1=1
			elif [ $value == 2 ]
			then
				bFoundType2=1
			fi
		done

		# return to encryption object
		json_select ..

		# select the authentication type based on the wpa values that were found
		if [ $bFoundType1 == 1 ]
		then
			auth="psk"
		fi
		if [ $bFoundType2 == 1 ]
		then
			# wpa2 overrides wpa
			auth="psk2"
		fi

	fi
}

# read network encryption type from json data from iwinfo scan
_UserInputJsonReadNetworkAuth () {
	# select the encryption object
	local type=""
	json_get_type type encryption

	# read the encryption object
	if [ "$type" == "object" ]
	then
		# select the encryption object
		json_select encryption

		# read the authentication object type
		json_get_type type authentication
		if [ "$type" != "" ]
		then
			# read the authentication type
			json_select authentication
			json_get_keys auth_arr
			
			json_get_values auth_type 
			json_select ..

			# read psk specifics
			if [ "$auth_type" == "psk" ]
			then
				_UserInputJsonReadNetworkAuthPsk
			else
				auth=$auth_type
			fi
		else
			# no encryption, open network
			echo "no encryption"
			auth="none"
		fi
	else
		# no encryption, open network
		auth="none"
	fi
}

# manually read network authentication from user
_UserInputReadNetworkAuth () {
	echo ""
	echo "Select network authentication type:"
	echo "1) WPA2"
	echo "2) WPA"
	echo "3) WEP"
	echo "4) none"
	echo ""
	echo -n "Selection: "
	read input
	

	case "$input" in
    	1)
			auth="psk2"
	    ;;
	    2)
			auth="psk"
	    ;;
	    3)
			auth="wep"
	    ;;
	    4)
			auth="none"
	    ;;
	esac

}


# scan wifi networks, display for user, allow them to pick one
_UserInputScanWifi () {
	# run the scan command and get the response
	local RESP=$(ubus call iwinfo scan '{"device":"wlan0"}')
	
	# read the json response
	json_load "$RESP"
	
	# check that array is returned  
	json_get_type type results

	# find all possible keys
	json_select results
	json_get_keys keys
	
	
	if 	[ "$type" == "array" ] &&
		[ "$keys" != "" ];
	then
		echo ""
		echo "Select Wifi network:"
		
		# loop through the keys
		for key in $keys
		do
			# select the array element
			json_select $key
			
			# find the ssid
			json_get_var cur_ssid ssid
			if [ "$cur_ssid" == "" ]
			then
				cur_ssid="[hidden]"
			fi
			echo "$key) $cur_ssid"

			# return to array top
			json_select ..
		done

		# read the input
		echo ""
		echo -n "Selection: "
		read input;
		
		# get the selected ssid
		json_select $input
		json_get_var ssid ssid
		
		echo "Network: $ssid"

		# detect the encryption type 
		_UserInputJsonReadNetworkAuth

		echo "Authentication type: $auth"
	else
		wifi
		bScanFailed=1
		echo "> ERROR: Scan failed, try again"
	fi
}

# main function to read user input
_UserInputMain () {
	bScanFailed=0
	echo "Onion Omega Wifi Setup"
	echo ""
	echo "Select from the following:"
	echo "1) Scan for Wifi networks"
	echo "2) Type network info"
	echo "q) Exit"
	echo ""
	echo -n "Selection: "
	read input

	# choice between scanning 
	if [ $input == 1 ]
	then
		# perform the scan and select network
		echo "Scanning for wifi networks..."
		_UserInputScanWifi

	elif [ $input == 2 ]
	then
		# manually read the network name
		echo -n "Enter network name: "
		read ssid;

		# read the authentication type
		_UserInputReadNetworkAuth
	else
		echo "Bye!"
		exit
	fi

	# read the network password
	if 	[ "$auth" != "none" ] &&
		[ $bScanFailed == 0 ];
	then
		echo -n "Enter password: "
		read password
	fi

	echo ""
}



###########################
##### Parse Arguments #####

# parse arguments
while [ "$1" != "" ]
do
	case "$1" in
		# options
		-v|--v|-verbose|verbose)
			bVerbose=1
			shift
		;;
		-ap|--ap|accesspoint|-accesspoint|--accesspoint)
			bApNetwork=1
			shift
		;;
		# commands
    	-add|add|-edit|edit)
			bCmd=1
			bCmdAdd=1
			shift
		;;
		-disable|disable)
			bCmd=1
			bCmdDisable=1
			shift
		;;
		-remove|remove)
			bCmd=1
			bCmdRemove=1
			shift
		;;
		-priority|priority)
			bCmd=1
			bCmdPriority=1
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
		-move|move)
			shift
			priorityMove=$1
			shift
		;;
	    *)
			echo "ERROR: Invalid Argument: $1"
			shift
		;;
	esac
done



########################
########################
##### Main Program #####

## user input ##
if [ $bCmd == 0 ]; then
	_UserInputMain

	# enable the add command if user input was successful (ssid and auth are defined)
	if 	[ "$ssid" != "" ] &&
		[ "$auth" != "" ];
	then
		bCmdAdd=1
	fi
fi


## parameter processing
if [ $bApNetwork == 1 ]; then
	networkType="ap"
	id=$(_FindApNetwork)
	ssid=$(_FindNetworkSsid)
else
	networkType="sta"
	# check if network already exists in configuration
	id=$(_FindNetworkBySsid "$ssid")
fi

if [ "$auth" != "" ]; then
	_NormalizeAuthInput
fi



## commands
if [ $bCmdAdd == 1 ]; then
	# if it doesn't already exist, add a new section
	if [ $id == -1 ]; then
		id=$(_FindNumNetworks)
	fi

	# add or edit the uci entry
	_AddWifiUciSection $id $networkType

	# set new AP networks to top in list
	if [ $bApNetwork == 1 ]; then
		_ReorderWifiUciSection $id 1
	fi

elif [ $bCmdDisable == 1 ]; then
	# only disable existing networks
	if [ $id != -1 ]; then
		_DisableWifiUciSection $id
	fi

elif [ $bCmdRemove == 1 ]; then
	# only remove existing networks
	if [ $id != -1 ]; then
		_DeleteWifiUciSection $id
	fi

elif [ $bCmdPriority == 1 ]; then
	# only move existing network
	if [ $id != -1 ]; then
		_SetNetworkPriority $id $priorityMove
	fi 
fi # command if else statement


# check that network was found
if [ $id == -1 ]; then
	echo "> ERROR: specified ssid not in the database"
fi

