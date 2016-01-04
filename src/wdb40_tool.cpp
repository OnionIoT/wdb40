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

	// deallocate the iwinfo object
	delete 	iw;

	return status;
}

int wdb40Tool::ReadConfigNetworks() 
{
	int 	status;

	_Print(1, "> Reading configurated networks...\n");

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