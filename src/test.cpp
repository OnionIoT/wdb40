#include <test.h>



int main(int argc, char **argv)
{
	int 		status;

	iwInfoIntf	*iw;

	// initialize the object
	iw 	= new iwInfoIntf();

	status = iw->ReadBackend();
	status |= iw->PrintScanList();

	// cleanup
	delete iw;

	return 0;
}