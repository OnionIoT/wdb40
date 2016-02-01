#include <cli.h>

int 	verbose;
int 	bPrintToFile;

void usage(const char* progName) 
{
	printf("WDB40: Making wireless connections easier\n");
	printf("\n");
	printf("Valid arguments:\n");
	printf("   init      - Read network configuration, disable all client networks\n");
	printf("   read      - Read network configuration\n");
	printf("   wait      - Wait for network wireless device status to be up\n");
	printf("   waitWwan  - Wait for network wwan interface status to be up\\n");
	printf("   scan      - Scan for available networks, check for match against configured networks\n");
	printf("   connect   - Connect to a matched network\n");
	printf("   disable   - Disable all client networks, enable AP network\n");
	printf("\n");
	printf("Valid options:\n");
	printf("   -v        Increase output verbosity \n");
	printf("   -q        Minimum verbosity \n");
	printf("   -t        Define timeout in secondsfor 'wait' and 'waitWwan' stages \n");
	printf("   -f        Force: make it mandatory to run wifi restart after setting up a network \n");
	printf("   -n        Do not print program info to file in ram \n");

	printf("\n");
}

// read the UCI network configuration
int readNetworkConfig() 
{
	int 		status;
	wdb40Tool	*wdb40;


	// initialize the object
	wdb40 		= new wdb40Tool();
	wdb40->SetVerbosity(verbose);

	// read configured networks
	status 	|= wdb40->ReadConfigNetworks(bPrintToFile);
	if (status != EXIT_SUCCESS) {
		printf("Returned ERROR!\n", status);
	}

	// clean-up
	delete 	wdb40;

	return status;
}

// read the UCI network configuration
// optionally enable the AP network
// bring down all the STAs
int networkSetup(int bEnableAp) 
{
	int 		status;
	wdb40Tool	*wdb40;


	// initialize the object
	wdb40 		= new wdb40Tool();
	wdb40->SetVerbosity(verbose);


	/*// wait until wireless status is up
	status 	= wdb40->WaitUntilWirelessStatus(1, WDB40_NETWORK_WIRELESS);
	if (status != EXIT_SUCCESS) {
		printf("Returned ERROR!\n", status);
	}*/

	// read configured networks
	status 	|= wdb40->ReadConfigNetworks(bPrintToFile);
	if (status != EXIT_SUCCESS) {
		printf("Returned ERROR!\n", status);
	}

	// enable AP wireless
	if (bEnableAp == 1) {
		status 	|= wdb40->SetApWirelessEnable(1);
		if (status != EXIT_SUCCESS) {
			printf("Returned ERROR!\n", status);
		}
	}

	// disable all STA networks
	status 	|= wdb40->SetAllStaWirelessEnable(0);
	if (status != EXIT_SUCCESS) {
		printf("Returned ERROR!\n", status);
	}


	//// reload the wifi configuration
	status 	|= wdb40->RestartWireless();


	// clean-up
	delete 	wdb40;

	return status;
}

// enable just the AP network, bring down all the STAs
int disableSta() 
{
	int 		status;
	wdb40Tool	*wdb40;


	// initialize the object
	wdb40 		= new wdb40Tool();
	wdb40->SetVerbosity(verbose);

	// read configured networks
	status 	|= wdb40->ReadConfigNetworks(bPrintToFile);
	if (status != EXIT_SUCCESS) {
		printf("Returned ERROR!\n", status);
	}

	// enable AP wireless
	status 	|= wdb40->SetApWirelessEnable(1);
	if (status != EXIT_SUCCESS) {
		printf("Returned ERROR!\n", status);
	}

	// disable all STA networks
	status 	|= wdb40->SetAllStaWirelessEnable(0);
	if (status != EXIT_SUCCESS) {
		printf("Returned ERROR!\n", status);
	}


	//// reload the wifi configuration
	status 	|= wdb40->RestartWireless();


	// clean-up
	delete 	wdb40;

	return status;
}

// wait until wifi device/intf is up
//	wireless device 	- overall radio0 is up
// 	wwan intf 			- if client connection is up
int waitUntilReady(int statusType, int timeoutSeconds)
{
	int 		status;
	wdb40Tool	*wdb40;


	// initialize the object
	wdb40 		= new wdb40Tool();
	wdb40->SetVerbosity(verbose);


	// wait until wireless status is up
	status 	= wdb40->WaitUntilWirelessStatus(1, statusType, timeoutSeconds);
	if (status != EXIT_SUCCESS) {
		printf("Returned ERROR!\n", status);
	}


	// clean-up
	delete 	wdb40;

	return status;
}

// scan for networks
int scanNetworks()
{
	int 		status;
	wdb40Tool	*wdb40;


	// initialize the object
	wdb40 		= new wdb40Tool();
	wdb40->SetVerbosity(verbose);

	// scan for networks
	status 	= wdb40->ScanAvailableNetworks(bPrintToFile);
	if (status != EXIT_SUCCESS) {
		printf("Returned ERROR!\n", status);
	}

	// read the configured networks from file
	status 	= wdb40->FetchConfigNetworks();
	if (status != EXIT_SUCCESS) {
		printf("Returned ERROR!\n", status);
	}

	// check configured networks against the scanned networks
	status 	= wdb40->CheckForConfigNetworks(bPrintToFile);
	if (status != EXIT_SUCCESS) {
		printf("Returned ERROR!\n", status);
		return 0;
	}

	// clean-up
	delete 	wdb40;

	return status;
}

// attempt to connect to a network
//	bForce 	- makes it mandatory to run wifi restart after setting up matched network
int connectAttempt(int bForce)
{
	int 		status;
	wdb40Tool	*wdb40;

	// initialize the object
	wdb40 		= new wdb40Tool();
	wdb40->SetVerbosity(verbose);

	// read match file
	status 	= wdb40->FetchMatchNetworks();
	if (status != EXIT_SUCCESS) {
		printf("Returned ERROR!\n", status);
		return EXIT_FAILURE;
	}

	// print the matches
	wdb40->PrintMatchNetworks();

	// enable matched network
	status 	= wdb40->EnableMatchedNetwork(bForce, bPrintToFile);
	if (status == EXIT_SUCCESS) {
		// restart wireless
		status 	|= wdb40->RestartWireless();
	}

	if (status != EXIT_SUCCESS) {
		printf("Returned ERROR!\n", status);
	}

	// clean-up
	delete 	wdb40;

	return status;
}

int main(int argc, char **argv)
{
	const char 	*progname;
	int 		status;
	int 		ch;
	int 		timeout;
	int 		bForce;

	// set defaults
	verbose 		= 1;
	bPrintToFile 	= 1;
	bForce 			= 0;
	timeout 		= WDB40_TOOL_TIMEOUT_DEFAULT_SECONDS;

	// save the program name
	progname 		= argv[0];	


	//// parse the option arguments
	while ((ch = getopt(argc, argv, "vqnfht:")) != -1) {
		switch (ch) {
		case 'v':
			// verbose output
			verbose++;
			break;
		case 'q':
			// quiet output
			verbose = 0;
			break;
		case 't':
			// define timeout seconds
			timeout = atoi(optarg);
			break;
		case 'f':
			// enable force
			bForce 	= 1;
			break;
		case 'n':
			// do not print to file
			bPrintToFile 	= 0;
			break;
		default:
			usage(progname);
			return 0;
		}
	}

	// advance past the option arguments
	argc 	-= optind;
	argv	+= optind;

	if (argc == 0) {
		usage(progname);
		return 0;
	}

	//// parse the arguments
	if (strcmp("init", argv[0]) == 0) {
		// prep for the scan
		status 	= networkSetup(0);	// do not enable the AP
		if (status != EXIT_SUCCESS) {
			printf("Returned ERROR!\n", status);
		}
	}
	else if (strcmp("read", argv[0]) == 0) {
		// read network config data
		status 	= readNetworkConfig();
		if (status != EXIT_SUCCESS) {
			printf("Returned ERROR!\n", status);
		}
	}
	else if (strcmp("wait", argv[0]) == 0) {
		// wait until wireless device is ready again
		status 	= waitUntilReady(WDB40_NETWORK_WIRELESS, timeout);
		if (status != EXIT_SUCCESS) {
			printf("Returned ERROR!\n", status);
		}
	}
	else if (strcmp("waitWwan", argv[0]) == 0) {
		// wait until network intf wwan (sta client connection) is ready again
		status 	= waitUntilReady(WDB40_NETWORK_INTF_WWAN, timeout);
		if (status != EXIT_SUCCESS) {
			printf("Returned ERROR!\n", status);
		}
	}
	else if (strcmp("scan", argv[0]) == 0) {
		// perform the scan
		status 	= scanNetworks();
		if (status != EXIT_SUCCESS) {
			printf("Returned ERROR!\n", status);
		}
	}
	else if (strcmp("connect", argv[0]) == 0) {
		// connect to a matched network
		status 	= connectAttempt(bForce);
		if (status != EXIT_SUCCESS) {
			printf("Returned ERROR!\n", status);
		}
	}
	else if (strcmp("disable", argv[0]) == 0) {
		// Disable all client networks, enable the AP
		status 	= networkSetup(1);
		if (status != EXIT_SUCCESS) {
			printf("Returned ERROR!\n", status);
		}
	}
	else {
		usage(progname);
	}


	return 0;
}