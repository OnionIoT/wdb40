#include <wdb40_tool.h>

wdb40Tool::wdb40Tool(void)
{
	Reset();
}

wdb40Tool::~wdb40Tool(void)
{
	// nothing for now
	if (uci != NULL) {
		delete uci;
	}
}


void wdb40Tool::Reset()
{
	// objects
	iw 			= NULL;
	uci 		= NULL;
	ubus 		= NULL;

	// vectors
	scanList.clear();
	configList.clear();
	matchList.clear();

	// other
	bWirelessConfigChanged 	= 0;
}


///// UCI INTF FUNCTIONS /////
// use uci intf to read configured networks
int wdb40Tool::ReadConfigNetworks(int bPrintToFile) 
{
	int 	status;

	_Print(1, "> Reading configured networks...\n");

	// initialize the object
	uci 	= new uciIntf();
	uci->SetVerbosity(verbosityLevel);

	// initialize the uci backend
	_Print(2, ">> Initializing the uci backend...\n");
	status 	= uci->ReadBackend();
	_Print(4, "\tstatus = %d\n", status);

	// read the wireless config section
	_Print(2, ">> Reading the wireless configuration...\n");
	status 	|= uci->ReadWirelessConfig();
	_Print(4, "\tstatus = %d\n", status);

	// return the processed vector of networks
	uci->GetNetworkList(configList);
	_Print(2, ">> List has %d networks\n", configList.size() );
	if (verbosityLevel > 2) {
		_PrintNetworkList(configList, 1);
	}

	// print the networks to a file
	if (bPrintToFile == 1) {
		_FilePrintNetworkList(configList, WDB40_TOOL_FILE_CONFIG, WDB40_TOOL_FILE_PATH, 1);
	}

	// release the uci context
	status 	|= uci->ReleaseBackend();

	// clean-up
	delete uci;
	uci 	= NULL;


	return status;
}

// enable/disable main AP network
int wdb40Tool::SetApWirelessEnable(int bEnable)
{
	int 	status 		= EXIT_FAILURE;
	int 	bDisable 	= (bEnable == 1 ? 0 : 1);		// uci option is disable, reverse of enable

	_Print(1, "> %s AP Network\n", (bEnable == 1 ? "Enabling" : "Disabling") );


	// initialize the object
	uci 	= new uciIntf();
	uci->SetVerbosity(verbosityLevel);

	// generic traversal of network list
	status 	= _GenericNetworkListTraversal(WDB40_TOOL_TRAVERSAL_ENABLE_AP, bDisable);

	// clean-up
	delete uci;
	uci 	= NULL;

	return 	status;
}

// disable all STA networks
int wdb40Tool::SetAllStaWirelessEnable (int bEnable)
{
	int 	status 		= EXIT_FAILURE;

	int 	bDisable 	= (bEnable == 1 ? 0 : 1);		// uci option is disable, reverse of enable

	_Print(1, "> %s all STA Networks\n", (bEnable == 1 ? "Enabling" : "Disabling") );


	// initialize the object
	uci 	= new uciIntf();
	uci->SetVerbosity(verbosityLevel);

	// generic traversal of network list
	status 	= _GenericNetworkListTraversal(WDB40_TOOL_TRAVERSAL_ENABLE_ALL_STA, bDisable);

	// clean-up
	delete uci;
	uci 	= NULL;

	return 	status;
}

int wdb40Tool::EnableMatchedNetwork(int bForce, int bPrintToFile)
{
	int 	status 	= EXIT_FAILURE;
	int 	currentDisabledStatus;

	if (matchList.size() > 0) {
		_Print(1, "> Attempting to enable wireless network configuration...\n");

		// initialize the object
		uci 	= new uciIntf();
		uci->SetVerbosity(verbosityLevel);

		// initialize the uci backend
		_Print(2, ">> Initializing the uci backend...\n");
		status 	= uci->ReadBackend();
		if (status != EXIT_SUCCESS) {
			return EXIT_FAILURE;
		}

		//// attempt to connect to the first matched network
		// check if network is already enabled
		_Print(2, ">> Checking if network is already enabled...\n");
		status 	= uci->GetWirelessSectionDisable( &(matchList.at(0)), currentDisabledStatus);

		// enable the network
		_Print(2, ">> Enabling wireless network '%s'\n", (matchList.at(0)).GetSsid().c_str() );
		status 	= uci->SetWirelessSectionDisable( &(matchList.at(0)), 0, 1);
		
		// trigger wifi restart (only if status has changed)
		if (status == EXIT_SUCCESS && currentDisabledStatus == 1 || bForce == 1) {
			bWirelessConfigChanged 	= 1;
		}


		// print the rest of the matched networks to disk (if any)
		if (bPrintToFile == 1) {
			// remove the first element from the vector
			matchList.erase ( matchList.begin() );

			// print the rest of the vector to the file
			_FilePrintNetworkList(matchList, WDB40_TOOL_FILE_MATCH);
		}


		// release the uci context
		status 	|= uci->ReleaseBackend();

		// clean-up
		delete uci;
		uci 	= NULL;
	}

	return 	status;
}


///// IWINFO INTF FUNCTIONS /////
// use iwinfo intf to scan for available networks
int wdb40Tool::ScanAvailableNetworks(int bPrintToFile)
{
	int 	status, tmp;

	// allocate new iwinfo object
	iw 		= new iwInfoIntf();
	iw->SetVerbosity(1);

	// perform the scan
	_Print(1, "> Scanning for available networks...\n");
	status 	= iw->WifiScan();

	// get the vector of networks
	status 	|= iw->GetScanList(scanList);
	_Print(2, ">> Found %d networks\n", scanList.size());

	// print the scanned networks to file
	if (bPrintToFile == 1) {
		_FilePrintNetworkList(scanList, WDB40_TOOL_FILE_SCAN);
	}

	// release the iwinfo backend
	iw->ReleaseBackend();


	// deallocate the iwinfo object
	delete 	iw;
	iw 	= NULL;

	return status;
}



///// UBUS INTF FUNCTIONS /////
// use ubus to find the status.up for network.wireless
int wdb40Tool::CheckWirelessStatus (int &bUp, int statusType)
{
	int 	status;

	// default
	bUp 	= 0;

	// allocate new ubus object
	ubus 	= new ubusIntf();
	ubus->SetVerbosity(verbosityLevel);

	_Print(3, "> Checking wireless %s status\n", (GetNetworkIntfString(statusType)).c_str() );

	// perform the check
	status 	= ubus->GetNetworkUpStatus(bUp, statusType);
	//_Print(2, ">> checked network.wireless, ret = %d, up = %d\n", status, bUp);

	if (status == EXIT_SUCCESS) {
		_Print(3, "> Wireless %s up = %d\n", (GetNetworkIntfString(statusType)).c_str(), bUp);
	}


	// deallocate the iwinfo object
	delete 	ubus;
	ubus 	= NULL;

	return status;
}

// wait until the network.wireless.status matches the argument
int wdb40Tool::WaitUntilWirelessStatus (int bUp, int statusType, int timeoutSeconds)
{
	int 	status;
	int 	bLoop, count, timeout, wirelessStatus;


	_Print(1, "> Waiting %d seconds for wireless status to be %d\n", timeoutSeconds, bUp);
	bLoop			= 1;
	count 			= 0;
	timeout 		= WDB40_TOOL_TIMEOUT_COUNT * timeoutSeconds;
	wirelessStatus 	= -1;

	// loop until the desired status is found (or timeout)
	while (bLoop) {
		// check the status
		status 	= CheckWirelessStatus(wirelessStatus, statusType);

		// delay between checks
		usleep(1000);

		// handle the looping
		count++;
		bLoop 	= (wirelessStatus != bUp && count < timeout);
	}

	// check for timeout
	if (count >= timeout) {
		_Print(1, "> ERROR: timeout waiting for wireless status!\n");
		return EXIT_FAILURE;
	}

	_Print(1, "> Done, wireless status is %d!\n", wirelessStatus);
	sleep(1);
	return 	status;
}


///// WDB40 FUNCTIONS /////
// NOTE: deprecated since cannot use uci and iwinfo libraries in same program
// compare configured networks against scanned networks
int wdb40Tool::CheckForConfigNetworks(int bPrintToFile)
{
	int 	status, bMatch;

	_Print(1, "> Checking for available configured networks\n");

	// check that the two lists are valid
	if (scanList.size() == 0) {
		_Print(1, "> No scanned networks found\n");
		return EXIT_FAILURE;
	}
	if (configList.size() == 0) {
		_Print(1, "> No configured networks found\n");
		return EXIT_FAILURE;
	}

	// loop through the configured network list
	for (	std::vector<networkInfo>::iterator itConfig = configList.begin(); 
			itConfig != configList.end(); 
			itConfig++) 
	{
		// check against all scanned networks
		for (	std::vector<networkInfo>::iterator itScan = scanList.begin(); 
				itScan != scanList.end(); 
				itScan++) 
		{
			// skip AP networks
			if ( (*itConfig).GetNetworkMode() == WDB40_NETWORK_AP ) {
				continue;
			}

			// compare the networks
			bMatch 	= 0;
			status 	= _CompareNetworks( (*itConfig), (*itScan), bMatch );

			// check if the networks match
			if (status == EXIT_SUCCESS && bMatch == 1) {
				//_Print(1, "> match for network '%s'\n", ((*itConfig).GetSsid()).c_str() );
				//(*itConfig).Print();

				// add network to match list
				matchList.push_back(*itConfig);
			}
		} // scanList loop
	} // configList loop

	if (matchList.size() > 0) {
		_Print(1, "> Found %d matching network%s!\n", matchList.size(), (matchList.size() > 1 ? "s" : "") );
	}

	// print the networks to a file
	if (bPrintToFile == 1) {
		_FilePrintNetworkList(matchList, WDB40_TOOL_FILE_MATCH);
	}

	return EXIT_SUCCESS;
}


// reload wireless
int wdb40Tool::RestartWireless (int bForce)
{
	if (bWirelessConfigChanged == 1 || bForce == 1) {
		_Print(1, "> Restarting wifi interface...\n");
		system("/sbin/wifi");
		//usleep(500000);
		sleep(2);

		bWirelessConfigChanged 	= 0;
	}

	return EXIT_SUCCESS;
}

void wdb40Tool::PrintMatchNetworks()
{
	if (matchList.size() > 0) {
		_Print(1, "> Matched networks:\n");
		_PrintNetworkList(matchList);
	}
	else {
		_Print(1, "> Scan found no matching networks!\n");
	}
}


///// FILE FUNCTIONS /////
int wdb40Tool::FetchConfigNetworks()
{
	_FileReadNetworkList(configList, WDB40_TOOL_FILE_CONFIG);
	//_PrintNetworkList(configList);

	return 	EXIT_SUCCESS;
}

int wdb40Tool::FetchScanNetworks()
{
	_FileReadNetworkList(scanList, WDB40_TOOL_FILE_SCAN);
	//_PrintNetworkList(scanList);

	return 	EXIT_SUCCESS;
}

int wdb40Tool::FetchMatchNetworks()
{
	_FileReadNetworkList(matchList, WDB40_TOOL_FILE_MATCH);
	//_PrintNetworkList(scanList);

	return 	EXIT_SUCCESS;
}



//// private functions
void wdb40Tool::_PrintNetworkList(std::vector<networkInfo> &networkList, int bExtendedPrint)
{
	for (	std::vector<networkInfo>::iterator it = networkList.begin(); 
			it != networkList.end(); 
			it++
		) 
	{
		if (bExtendedPrint == 0) {
			(*it).PrintBasic();
		}
		else {
			(*it).Print();
		}
	}
}

void wdb40Tool::_FilePrintNetworkList(std::vector<networkInfo> &networkList, std::string filename, std::string path, int bOnlyStaNetworks)
{
	char 			filePath[WDB40_MAX_STRING_SIZE];
	std::ofstream 	file;
	
	// open the file for writing
	sprintf(filePath, "%s/%s", path.c_str(), filename.c_str() );
	file.open( filePath );

	// print each network in the vector
	if (file.is_open() ) {
		for (	std::vector<networkInfo>::iterator it = networkList.begin(); 
				it != networkList.end(); 
				it++
			) 
		{
			if 	(	bOnlyStaNetworks == 0 ||
					(bOnlyStaNetworks == 1 && (*it).GetNetworkMode() == WDB40_NETWORK_STA)
				)
			{
				(*it).FilePrintBasic(file);
			}
		}
	}

	// close the file
	file.close();

	_Print(2, "> Printed network list to file '%s'\n", filePath);
}

int wdb40Tool::_FileReadNetworkList(std::vector<networkInfo> &networkList, std::string filename, std::string path)
{
	int 	status 		= EXIT_FAILURE;
	int 	encrType;
	char 	line1[WDB40_MAX_STRING_SIZE];
	char 	line2[WDB40_MAX_STRING_SIZE];
	char 	filePath[WDB40_MAX_STRING_SIZE];

	std::string 	ssid;
	std::ifstream 	file;
	
	// open the file for reading
	sprintf(filePath, "%s/%s", path.c_str(), filename.c_str() );
	file.open( filePath );

	// parse each network from the file
	if (file.is_open() ) {
		_Print(2, ">> Successfully opened file '%s'\n", filePath );
		// file exists
		while( 	file.getline(line1, WDB40_MAX_STRING_SIZE) &&
				file.getline(line2, WDB40_MAX_STRING_SIZE)) 
		{
			//_Print(3, "parsing! '%s' and '%s'\n", line1, line2);
			// parse the read lines
			status 	= networkInfo::ParseNetworkFileLine (line1, line2, ssid, encrType);

			if (status == EXIT_SUCCESS) {
				//_Print(1, "read: ssid: '%s', encryption type: '%d'\n", ssid.c_str(), encrType);
				_AddNetwork(networkList, ssid, encrType);
			}
		}
	}

	// close the file
	file.close();

	return 	status;
}


// compare two networks based on SSID and encryption type
// 	comparison result 		bMatch
//	different 				0
//	same 					1
int wdb40Tool::_CompareNetworks (networkInfo network1, networkInfo network2, int &bMatch)
{
	std::string 	ssid1, ssid2;

	// default value
	bMatch 	= 0;

	// populate the ssid values
	ssid1 	= network1.GetSsid();
	ssid2 	= network2.GetSsid();

	// check that both networks have data
	if 	( 	ssid1.compare(NETWORK_INFO_DEFAULT_NONE) == 0 || 
			ssid2.compare(NETWORK_INFO_DEFAULT_NONE) == 0
		)
	{
		
		return 	EXIT_FAILURE;
	}

	// compare the networks
	_Print(3, ">> Comparing ssid '%s':%d against ssid '%s':%d\n", ssid1.c_str(), network1.GetEncryptionType(), ssid2.c_str(), network2.GetEncryptionType() );
	if 	( 	ssid1.compare(ssid2) == 0 &&
			network1.GetEncryptionType() == network2.GetEncryptionType()
		)
	{
		// ssid and encryption type match, networks are the same
		bMatch 	= 1;
	}

	return EXIT_SUCCESS;
}

// generic function for enabling/disabling wireless network based on configList vector
int wdb40Tool::_GenericNetworkListTraversal(int mode, int param0)
{
	int 	status 		= EXIT_FAILURE;

	// initialize the uci backend
	status 	= uci->ReadBackend();
	if (status != EXIT_SUCCESS) {
		return EXIT_FAILURE;
	}

	// loop through the configured network list
	for (	std::vector<networkInfo>::iterator itConfig = configList.begin(); 
			itConfig != configList.end(); 
			itConfig++) 
	{
		// GENERIC:: perform action based on the mode
		if (mode == WDB40_TOOL_TRAVERSAL_ENABLE_AP) {
			// only look for AP wireless network
			if ( (*itConfig).GetNetworkMode() == WDB40_NETWORK_AP) {
				
				// check if uci call is required (current enabled state is not the same as requested state)
				if ((*itConfig).GetDisabled() != param0) {
					status 	= uci->SetWirelessSectionDisable( &(*itConfig), param0, 1);
					_Print(3, ">>> SetWirelessSectionDisable returned %d\n", status);

					// note that wireless config has changed
					bWirelessConfigChanged	= 1;
				}

				// finish the loop after the first AP network
				break;
			}
		}
		else if (mode == WDB40_TOOL_TRAVERSAL_ENABLE_ALL_STA) {
			// only look for STA wireless networks
			if ( (*itConfig).GetNetworkMode() == WDB40_NETWORK_STA) {

				// check if uci call is required (current enabled state is not the same as requested state)
				if ((*itConfig).GetDisabled() != param0) {
					status 	|= uci->SetWirelessSectionDisable( &(*itConfig), param0);
					_Print(3, ">>> SetWirelessSectionDisable returned %d\n", status);

					// note that wireless config has changed
					bWirelessConfigChanged	= 1;
				}
			}
		}
	} // configList loop

	// GENERIC:: perform post loop action based on the mode
	if (mode == WDB40_TOOL_TRAVERSAL_ENABLE_ALL_STA) {
		// disable all STA: need to commit changes (if any were made)
		if (bWirelessConfigChanged == 1) {
			status 	|= uci->CommitSectionChanges();
		}
	}

	// release the uci context
	status 	|= uci->ReleaseBackend();

	return 	status;
}

// given a scanned network's ssid and encryption type, 
// compare against all of the configured networks to find a match
void wdb40Tool::_AddNetwork (std::vector<networkInfo> &networkList, std::string ssid, int encrType)
{
	int 	status;
	networkInfo 	*network;

	_Print(3, ">> Adding network '%s' to network list\n", ssid.c_str() );
	network 	= new networkInfo(ssid, encrType);
	network->SetVerbosity(1);

	// add to the list
	networkList.push_back(*network);

	// clean-up
	delete 	network;
}
