#include <test.h>



int main(int argc, char **argv)
{
	int 		status, tmp;

	wdb40Tool	*wdb40;


	//// TESTING WDB40 TOOL
	// initialize the object
	wdb40 		= new wdb40Tool();
	wdb40->SetVerbosity(2);

	// read configured networks
	status 	= wdb40->ReadConfigNetworks();
	if (status != EXIT_SUCCESS) {
		printf("Returned ERROR!\n", status);
		return 0;
	}

	// enable AP wireless
	status 	= wdb40->SetApWirelessEnable(1);
	if (status != EXIT_SUCCESS) {
		printf("Returned ERROR!\n", status);
		return 0;
	}

	// disable all STA networks
	status 	= wdb40->SetAllStaWirelessEnable(0);
	if (status != EXIT_SUCCESS) {
		printf("Returned ERROR!\n", status);
		return 0;
	}

	// reload the wifi configuration
	status 	= wdb40->ReloadWifi();


	// scan for networks
	status 	= wdb40->ScanAvailableNetworks();
	if (status != EXIT_SUCCESS) {
		printf("Returned ERROR!\n", status);
		return 0;
	}

	// check configured networks against the scanned networks
	status 	= wdb40->CheckForConfigNetworks();
	if (status != EXIT_SUCCESS) {
		printf("Returned ERROR!\n", status);
		return 0;
	}


	// cleanup
	delete wdb40;

	return 0;
}