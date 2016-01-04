#include <wdb40_tool.h>

wdb40Tool::wdb40Tool(void)
{
	Reset();
}

wdb40Tool::~wdb40Tool(void)
{
	// nothing for now
}


void wdb40Tool::Reset()
{
	// objects
	iw 			= NULL;
	uci 		= NULL;

	// vectors
	scanList.clear();
	configList.clear();
}


// use iwinfo intf to scan for available networks
int wdb40Tool::ScanAvailableNetworks()
{
	int 	status, tmp;

	// allocate new iwinfo object
	iw 		= new iwInfoIntf();

	// perform the scan
	_Print(1, "> Scanning for available networks...\n");
	status 	= iw->WifiScan();

	// get the vector of networks
	status 	|= iw->GetScanList(scanList);
	_Print(2, ">> Found %d networks\n", scanList.size());

	//_PrintNetworkList(scanList);

	// deallocate the iwinfo object
	delete 	iw;

	return status;
}

// use uci intf to read configured networks
int wdb40Tool::ReadConfigNetworks() 
{
	int 	status;

	_Print(1, "> Reading configured networks...\n");

	// initialize the object
	uci 	= new uciIntf();

	// initialize the uci backend
	_Print(2, ">> Initializing the uci backend...\n");
	status = uci->ReadBackend();

	// read the wireless section
	_Print(2, ">> Reading the wireless configuration...\n");
	status = uci->ReadWirelessConfig();

	// process the wireless section
	_Print(2, ">> Processing the wireless configuration...\n");
	status = uci->ProcessConfigData();

	// return the processed vector of networks
	uci->GetNetworkList(configList);
	_Print(2, ">> List has %d networks\n", configList.size() );


	return status;
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

