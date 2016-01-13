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
}


///// UCI INTF FUNCTIONS /////
// use uci intf to read configured networks
int wdb40Tool::ReadConfigNetworks() 
{
	int 	status;

	_Print(1, "> Reading configured networks...\n");

	// initialize the object
	uci 	= new uciIntf();

	// initialize the uci backend
	_Print(2, ">> Initializing the uci backend...\n");
	status 	= uci->ReadBackend();
	_Print(4, "\tstatus = %d\n", status);

	// read the wireless section
	_Print(2, ">> Reading the wireless configuration...\n");
	status 	|= uci->ReadWirelessConfig();
	_Print(4, "\tstatus = %d\n", status);

	/*// process the wireless section
	_Print(2, ">> Processing the wireless configuration...\n");
	status 	|= uci->ProcessConfigData();
	_Print(4, "\tstatus = %d\n", status);*/

	// return the processed vector of networks
	uci->GetNetworkList(configList);
	_Print(2, ">> List has %d networks\n", configList.size() );

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

	// generic traversal of network list
	status 	= _GenericNetworkListTraversal(WDB40_TOOL_TRAVERSAL_ENABLE_ALL_STA, bDisable);

	// clean-up
	delete uci;
	uci 	= NULL;

	return 	status;
}



///// IWINFO INTF FUNCTIONS /////
// use iwinfo intf to scan for available networks
int wdb40Tool::ScanAvailableNetworks()
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

	_FilePrintNetworkList(scanList, WDB40_TOOL_FILE);
	iw->ReleaseBackend();


	// deallocate the iwinfo object
	delete 	iw;
	iw 	= NULL;

	return status;
}



///// UBUS INTF FUNCTIONS /////
// use ubus to find the status.up for network.wireless
int wdb40Tool::CheckWirelessStatus (int &bUp)
{
	int 	status;

	// default
	bUp 	= 0;

	// allocate new ubus object
	ubus 	= new ubusIntf();
	ubus->SetVerbosity(verbosityLevel);

	// perform the check
	_Print(3, "> Checking wireless status\n");
	status 	= ubus->GetNetworkWirelessUpStatus(bUp);
	//_Print(2, ">> checked network.wireless, ret = %d, up = %d\n", status, bUp);

	if (status == EXIT_SUCCESS) {
		_Print(3, "> Network.wireless up = %d\n", bUp);
	}


	// deallocate the iwinfo object
	delete 	ubus;
	ubus 	= NULL;

	return status;
}

// wait until the network.wireless.status matches the argument
int wdb40Tool::WaitUntilWirelessStatus (int bUp)
{
	int 	status;
	int 	bLoop, count, timeout, wirelessStatus;


	_Print(1, "> Waiting for wireless status = %d\n", bUp);
	bLoop			= 1;
	count 			= 0;
	timeout 		= WDB40_TOOL_TIMEOUT;
	wirelessStatus 	= -1;

	// loop until the desired status is found (or timeout)
	while (bLoop) {
		// check the status
		status 	= CheckWirelessStatus(wirelessStatus);

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
int wdb40Tool::ReadScanFile()
{
	_FileReadNetworkList(WDB40_TOOL_FILE);
}

// compare configured networks against scanned networks
int wdb40Tool::CheckForConfigNetworks()
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
				_Print(1, "> match for network '%s'\n", ((*itConfig).GetSsid()).c_str() );
				(*itConfig).Print();

				// LAZAR : add match to vector of matches
			}
		} // scanList loop
	} // configList loop

	return EXIT_SUCCESS;
}


// reload wireless
int wdb40Tool::RestartWireless ()
{
	_Print(1, "> Restarting wifi interface...\n");
	system("/sbin/wifi");
	//usleep(500000);
	sleep(2);

	return EXIT_SUCCESS;
}




//// private functions
void wdb40Tool::_PrintNetworkList(std::vector<networkInfo> networkList)
{
	for (	std::vector<networkInfo>::iterator it = networkList.begin(); 
			it != networkList.end(); 
			it++
		) 
	{
		(*it).PrintBasic();
	}
}

void wdb40Tool::_FilePrintNetworkList(std::vector<networkInfo> networkList, std::string filename)
{
	std::ofstream file;
	
	// open the file for writing
	file.open( filename.c_str() );

	// print each network in the vector
	if (file.is_open() ) {
		for (	std::vector<networkInfo>::iterator it = networkList.begin(); 
				it != networkList.end(); 
				it++
			) 
		{
			(*it).FilePrintBasic(file);
		}
	}

	// close the file
	file.close();
}

int wdb40Tool::_FileReadNetworkList(std::string filename)
{
	int 	status;
	int 	encrType;
	char 	line[IWINFO_MAX_STRING_SIZE];

	std::string 	ssid;
	std::ifstream 	file;
	
	// open the file for reading
	file.open( filename.c_str() );

	// print each network in the vector
	if (file.is_open() ) {
		_Print(2, ">> Successfully opened file '%s'\n", filename.c_str() );
		// file exists
		while (file.getline(line, IWINFO_MAX_STRING_SIZE) ) {
			// parse the line
			status 	= networkInfo::ParseNetworkFileLine (line, ssid, encrType);
			if (status == EXIT_SUCCESS) {
				_Print(1, "read: ssid: '%s', encryption type: '%d'\n", ssid.c_str(), encrType);
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
				status 	= uci->SetWirelessSectionDisable( &(*itConfig), param0, 1);
				_Print(3, ">>> SetWirelessSectionDisable returned %d\n", status);

				// finish the loop after the first AP network
				break;
			}
		}
		else if (mode == WDB40_TOOL_TRAVERSAL_ENABLE_ALL_STA) {
			// only look for STA wireless networks
			if ( (*itConfig).GetNetworkMode() == WDB40_NETWORK_STA) {
				status 	|= uci->SetWirelessSectionDisable( &(*itConfig), param0);
				_Print(3, ">>> SetWirelessSectionDisable returned %d\n", status);
			}
		}
	} // configList loop

	// GENERIC:: perform post loop action based on the mode
	if (mode == WDB40_TOOL_TRAVERSAL_ENABLE_ALL_STA) {
		// disable all STA: need to commit changes
		status 	|= uci->CommitSectionChanges();
	}

	// release the uci context
	status 	|= uci->ReleaseBackend();

	return 	status;
}



