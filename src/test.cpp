#include <test.h>



int main(int argc, char **argv)
{
	int 		status, tmp;

	iwInfoIntf	*iw;
	uciIntf 	*uci;

	//// TESTING IWINFO INTF
	// initialize the object
	iw 		= new iwInfoIntf();

	// do the scan
	printf("> Scanning for networks...\n");
	status = iw->WifiScan();

	// print the results
	iw->GetScanListSize(tmp);
	printf("> Found %d networks:\n", tmp);
	iw->PrintScanList();

	printf("\n");


	//// TESTING UCI INTF
	// initialize the object
	uci 	= new uciIntf();

	// init
	printf("> Initializing the uci backend...\n");
	status = uci->ReadBackend();
	printf("   result is %d\n", status);

	// do the scan
	printf("> Reading the wireless configuration...\n");
	status = uci->ReadWirelessConfig();
	printf("   result is %d\n", status);

	// print the results
	printf("> Processing the wireless configuration...\n");
	status = uci->ProcessConfigData();
	printf("   result is %d\n", status);


	// cleanup
	delete iw;
	delete uci;

	return 0;
}