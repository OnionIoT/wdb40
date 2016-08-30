#!/bin/sh

## script to setup uci wireless configuration for use with wdb40 utility

. /usr/share/libubox/jshn.sh
# include the Onion sh lib
. /usr/lib/onion/lib.sh

### global variables
# options
bVerbose=0
bJson=0
bError=0

#commands
bCmd=0
bCmdAdd=0
bCmdDisable=0
bCmdEnable=0
bCmdRemove=0
bCmdPriority=0
bCmdList=0
bCmdInfo=0


#parameters
bApNetwork=0


#############################
##### Print Usage ###########
usage () {
	_Print "Functionality:"
	_Print "	Configure WiFi networks on the Omega"
	_Print ""

	_Print "Interactive Usage:"
	_Print "$0"
	_Print "	Accepts user input"
	_Print ""
	_Print ""

	_Print "Command Line Usage:"
	_Print "$0 <command> <parameters>"
	_Print ""
	_Print "Available Commands:"
	_Print "  add "
	_Print "Functionality: Add a new WiFi network to the Omega's settings"
	_Print "Usage: $0 add -ssid <ssid> -encr <encryption type> -password <password>"
	_Print ""
	_Print "  edit "
	_Print "Functionality: Edit the information of a configured WiFi network"
	_Print "Usage: $0 edit -ssid <ssid> -encr <encryption type> -password <password>"
	_Print ""
	_Print "  remove "
	_Print "Functionality: Remove an existing WiFi network from the Omega's settings"
	_Print "Usage: $0 remove -ssid <ssid>"
	_Print ""
	_Print "  priority "
	_Print "Functionality: Move a WiFi network up or down in the priority list when attempting to connect"
	_Print "Usage: $0 priority -ssid <ssid> -move <up|down>"
	_Print "           up:     increase the priority"
	_Print "           down:   decrease the priority"
	_Print ""
	_Print "  list "
	_Print "Functionality: Display a JSON-formatted list of all configured networks"
	_Print "Usage: $0 list"
	_Print ""
	_Print "  info "
	_Print "Functionality: Display a JSON-formatted table of all info for specified network"
	_Print "Usage: $0 info -ssid <ssid>"
	_Print ""

	_Print ""
	_Print "Command Line Options:"
	_Print "  -v      Increase output verbosity"
	_Print "  -j      Set all output to JSON"
	_Print "  -ap     Set any commands above to refer to an AP network"
	_Print ""

	_Print ""
	_Print "Run 'wdb40' to make network configuration changes take effect"
	_Print ""
}


#############################
##### General Functions #####
# initialize the json
_Init () {
	if [ $bJson == 1 ]; then
		# json setup      
		json_init
	fi
}

# prints a message, taking json output into account
#	$1	- the message to print
#	$2	- the json index string
_Print () {
	if [ $bJson == 0 ]; then
		echo $1
	else 
		json_add_string "$2" "$1"
	fi
}

# set an error flag
_SetError () {
	bError=1
}

# close and print the json
_Close () {
	if [ $bJson == 1 ]; then
		# print the error status
		local output=$((!$bError))
		json_add_boolean "success" $output

		# print the json
		json_dump
	fi
}


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
#	returns value via echo
#	$1	- network id
_FindNetworkSsid () {
	local name=$(uci -q get wireless.\@wifi-iface[$1].ssid)

	echo $name
}

# Normalize the authentication input
#	modifies the global auth variable
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
			_Print "> Adding '$ssid' network to database (priority: $id) " "output"
		else
			_Print "> Editing '$ssid' network (priority: $id)" "output"
		fi
		# use UCI to set the network to client mode and wwan
		uci set wireless.@wifi-iface[$id].mode="sta"
		uci set wireless.@wifi-iface[$id].network="wwan"
		uci set wireless.@wifi-iface[$id].disabled="1"

		# TO DO: once firmware is stable, potentially remove this commented out code
		# if [ "$(GetDeviceType)" == "$DEVICE_OMEGA2" ];
		# then
		# 	#echo "Setting ifname for Omega2"
		# 	uci set wireless.@wifi-iface[$id].ifname="apcli0"
		# fi
	elif [ "$networkType" = "ap" ]; then
		_Print "> Setting up $ssid Access Point as network $id" "output"
		# use UCI to set the network to access-point mode and wlan
		uci set wireless.@wifi-iface[$id].mode="ap"
		uci set wireless.@wifi-iface[$id].network="wlan"

		# TO DO: once firmware is stable, potentially remove this commented out code
		# if [ "$(GetDeviceType)" == "$DEVICE_OMEGA2" ];
		# then
		# 	#echo "Setting ifname for Omega2"
		# 	uci set wireless.@wifi-iface[$id].ifname="ra0"
		# fi
	fi 

	# use UCI to set the ssid, encryption, and disabled options
	uci set wireless.@wifi-iface[$id].ssid="$ssid"
	uci set wireless.@wifi-iface[$id].encryption="$auth"
	
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
		$(uci commit wireless)
	fi

	# restart wifi intf if adding AP network
	if 	[ "$networkType" = "ap" ] &&
		[ $bNew == 1 ]; 
	then
		/etc/init.d/network restart
	fi
}

# disable a uci wifi network section
#	$1 - iface number
#	$2 - value to set to disabled option (is 1 by default)
_DisableWifiUciSection () {
	local param="1"
	local action="Disabling"

	if [ "$2" == "0" ]; then
		param="0"
		action="Enabling"
	fi

	# check the argument
	if [ $1 -ge 0 ]; then
		# ensure that iface exists
		local iface=$(uci -q get wireless.\@wifi-iface[$1])
		if [ "$iface" == "wifi-iface" ]; then
			_Print "> $action '$ssid' network" "output"
			uci set wireless.@wifi-iface[$1].disabled="$param"
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
			_Print "> Removing '$ssid' network from database" "output"
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
				_Print "> Shifting '$ssid' priority to $3" "output"
			else
				_Print "> Shifting '$ssid' priority" "output"
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
		_Print "> ERROR: Invalid priority shift requested" "error"
		_SetError
	else
		_ReorderWifiUciSection $id $desiredPriority $hmnPriority
	fi
}

# output a JSON list of configured networks
_JsonListUciNetworks () {
	local count=0

	# json setup      
	json_init
	
	# create the results array
	json_add_array results

	# find the first network
	local network=$(uci -q get wireless.\@wifi-iface[$count])

	# loop through all configured networks
	while [ "$network" == "wifi-iface" ]
	do
		# find the data
		local ssidRd=$(uci -q get wireless.\@wifi-iface[$count].ssid)
		local modeRd=$(uci -q get wireless.\@wifi-iface[$count].mode)
		local encrRd=$(uci -q get wireless.\@wifi-iface[$count].encryption)
		local passwordRd=$(uci -q get wireless.\@wifi-iface[$count].key)
		
		if [ "$encrRd" == "wep" ]; then
			passwordRd=$(uci -q get wireless.\@wifi-iface[$count].key$passwordRd)
		fi

		# create and populate object for this network
		json_add_object
		json_add_string "ssid" "$ssidRd"
		json_add_string "encryption" "$encrRd"
		json_add_string "password" "$passwordRd"
		json_add_string "mode" "$modeRd"
		json_close_object

		# continue the loop
		count=$(($count + 1))
		network=$(uci -q get wireless.\@wifi-iface[$count])
	done

	# finish the array
	json_close_array

	# print the json
	if [ $bJson == 0 ]; then
		json_dump
	fi
}

# output a JSON object of specified network
#	$1 	- network id
_JsonUciNetworkInfo () {
	local id=$1

	# json setup      
	json_init

	# find the first network
	local network=$(uci -q get wireless.\@wifi-iface[$id])

	# check the network and input parameter
	if 	[ "$network" == "wifi-iface" ] &&
		[ $id -ge 0 ]; 
	then
		# find the data
		local ssidRd=$(uci -q get wireless.\@wifi-iface[$id].ssid)
		local modeRd=$(uci -q get wireless.\@wifi-iface[$id].mode)
		local encrRd=$(uci -q get wireless.\@wifi-iface[$id].encryption)
		local passwordRd=$(uci -q get wireless.\@wifi-iface[$id].key)
		
		if [ "$encrRd" == "wep" ]; then
			passwordRd=$(uci -q get wireless.\@wifi-iface[$id].key$passwordRd)
		fi

		# create and populate object for this network
		json_add_boolean "success" 1
		json_add_string "ssid" "$ssidRd"
		json_add_string "encryption" "$encrRd"
		json_add_string "password" "$passwordRd"
		json_add_string "mode" "$modeRd"
		
	else
		json_add_boolean "success" 0
	fi

	# print the json
	json_dump
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

	# TO DO: once firmware is stable, potentially remove this commented out code	
	# if [ "$(GetDeviceType)" == "$DEVICE_OMEGA" ] 
	# then
	# 	#$(ifconfig wlan0 up)
	# 	RESP=$(ubus call iwinfo scan '{"device":"wlan0"}')
	# elif [ "$(GetDeviceType)" == "$DEVICE_OMEGA2" ]
	# then
	# 	# Need to run this command, for some reason it allows you to see al thhe networks
	# 	RESP=$(ubus call iwinfo scan '{"device":"apcli0"}')
	# fi
	
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
		-j|--j|-json|--json|json)
			bJson=1
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
		-enable|enable)
			bCmd=1
			bCmdEnable=1
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
		-list|list)
			bCmd=1
			bCmdList=1
			shift
		;;
		-info|info)
			bCmd=1
			bCmdInfo=1
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
		-h|--h|help|-help|--help)
			usage
			exit
		;;
	    *)
			echo "ERROR: Invalid Argument: $1"
			usage
			exit
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


## json init
_Init

## parameter processing
if [ $bApNetwork == 1 ]; then
	networkType="ap"
	id=$(_FindApNetwork)
	
	if 	[ $id != -1 ] &&
		[ "$ssid" == "" ]; 
	then
		ssid=$(_FindNetworkSsid)
	fi

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

elif [ $bCmdEnable == 1 ]; then
	# only enable existing networks
	if [ $id != -1 ]; then
		_DisableWifiUciSection $id 0
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

elif [ $bCmdList == 1 ]; then
	_JsonListUciNetworks

	# remove error message
	id=0

elif [ $bCmdInfo == 1 ]; then
	_JsonUciNetworkInfo $id

	# remove error message (will be printed in json)
	id=0
	
fi # command if else statement


# check that network was found
if [ $id == -1 ]; then
	_Print "> ERROR: specified ssid not in the database" "error"
	_SetError
fi

# scan and connect to available networks
if 	[ $bCmdAdd == 1 ] || 
	[ $bCmdDisable == 1 ] ||
	[ $bCmdEnable == 1 ] ||
	[ $bCmdRemove == 1 ] ||
	[ $bCmdPriority == 1 ];
then
	_Print "> Restarting wifi interface for changes to take effect" "status"
	wdb40 &
fi


## json finish
_Close

