#include <test.h>

int verbose;

void usage(const char* progName) 
{
	printf("NEED ARGUMENT!\n");
}

// enable just the AP network, bring down all the STAs
int initialSetup() 
{
	int 		status;
	wdb40Tool	*wdb40;


	// initialize the object
	wdb40 		= new wdb40Tool();
	wdb40->SetVerbosity(verbose);


	// wait until wireless status is up
	status 	= wdb40->WaitUntilWirelessStatus(1);
	if (status != EXIT_SUCCESS) {
		printf("Returned ERROR!\n", status);
	}

	// read configured networks
	status 	|= wdb40->ReadConfigNetworks();
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

// wait until wifi is up
int waitUntilReady()
{
	int 		status;
	wdb40Tool	*wdb40;


	// initialize the object
	wdb40 		= new wdb40Tool();
	wdb40->SetVerbosity(verbose);


	// wait until wireless status is up
	status 	= wdb40->WaitUntilWirelessStatus(1);
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

	//// scan for networks
	status 	= wdb40->ScanAvailableNetworks();
	if (status != EXIT_SUCCESS) {
		printf("Returned ERROR!\n", status);
		return 0;
	}

	// clean-up
	delete 	wdb40;

	return status;
}

// attempt to connect to a network
int connectAttempt()
{
	int 		status;
	wdb40Tool	*wdb40;


	// initialize the object
	wdb40 		= new wdb40Tool();
	wdb40->SetVerbosity(verbose);

	//// read scan file
	status 	= wdb40->ReadScanFile();
	if (status != EXIT_SUCCESS) {
		printf("Returned ERROR!\n", status);
		return 0;
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

	// set defaults
	verbose 	= 1;

	// save the program name
	progname 	= argv[0];	

	// save the program name
	progname 	= argv[0];	

	//// parse the option arguments
	while ((ch = getopt(argc, argv, "vqh:")) != -1) {
		switch (ch) {
		case 'v':
			// verbose output
			verbose++;
			break;
		case 'q':
			// quiet output
			verbose = 0;
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
	// first arg - command
	if (strcmp("init", argv[0]) == 0) {
		// prep for the scan
		status 	= initialSetup();
		if (status != EXIT_SUCCESS) {
			printf("Returned ERROR!\n", status);
		}
	}
	else if (strcmp("wait", argv[0]) == 0) {
		// wait until everyting is ready again
		status 	= waitUntilReady();
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
		// perform the scan
		status 	= connectAttempt();
		if (status != EXIT_SUCCESS) {
			printf("Returned ERROR!\n", status);
		}
	}
	else {
		usage(progname);
	}

	

	
	

	


/*
	wdb40Tool	*wdb40;

	// initialize the object
	wdb40 		= new wdb40Tool();
	wdb40->SetVerbosity(2);

	//// TESTING WDB40 TOOL
	// initialize the object
	wdb40 		= new wdb40Tool();
	wdb40->SetVerbosity(2);


	// wait until wireless status is up
	status 	= wdb40->WaitUntilWirelessStatus(1);
	if (status != EXIT_SUCCESS) {
		printf("Returned ERROR!\n", status);
		return 0;
	}

	// read configured networks
	status 	= wdb40->ReadConfigNetworks();
	if (status != EXIT_SUCCESS) {
		printf("Returned ERROR!\n", status);
		return 0;
	}
	return 0;

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

	
	//// reload the wifi configuration
	printf("\n");
	status 	= wdb40->RestartWireless();


	//// wait until wireless status is up
	status 	= wdb40->WaitUntilWirelessStatus(1);
	if (status != EXIT_SUCCESS) {
		printf("Returned ERROR!\n", status);
		return 0;
	}


	//// scan for networks
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
	delete wdb40;*/

	return 0;
}