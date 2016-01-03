#include <test.h>



int main(int argc, char **argv)
{
	int 		status, tmp;

	iwInfoIntf	*iw;

	// initialize the object
	iw 	= new iwInfoIntf();

	// do the scan
	printf("> Scanning for networks...\n");
	status = iw->WifiScan();

	// print the results
	iw->GetScanListSize(tmp);
	printf("> Found %d networks:\n", tmp);
	iw->PrintScanList();


	// cleanup
	delete iw;

	return 0;
}