#include <test.h>



int main(int argc, char **argv)
{
	int 		status, tmp;

	wdb40Tool	*wdb40;


	//// TESTING WDB40 TOOL
	// initialize the object
	wdb40 		= new wdb40Tool();
	wdb40->SetVerbosity(2);

	// scan for networks
	status 	= wdb40->ScanAvailableNetworks();
	printf("returned %d\n", status);

	// read configured networks
	status 	= wdb40->ReadConfigNetworks();
	printf("returned %d\n", status);

	// check configured networks against the scanned networks
	status 	= wdb40->CheckForConfigNetworks();
	printf("returned %d\n", status);
	

	// cleanup
	delete wdb40;

	return 0;
}