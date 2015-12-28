#include <iwinfo_intf.h>


iwInfoIntf::iwInfoIntf(char* device) 
{
	Reset();

	// initialize the device name
	wirelessDevice 	= new char[255];
	strncpy(wirelessDevice, device, strlen(device) );
}

iwInfoIntf::~iwInfoIntf() 
{
	// free dynamically allocated memory
	delete 	wirelessDevice;

	// iwinfo cleanup
	iwinfo_finish();
}


void iwInfoIntf::Reset()
{
	wirelessDevice 	= NULL;
}


int iwInfoIntf::ReadBackend()
{
	int 	status;

	// initialize the iw struct
	iw = iwinfo_backend(wirelessDevice);

	if (!iw)
	{
		// iw init failed
		return EXIT_FAILURE;
	}

	return status;
}

int iwInfoIntf::PrintScanList()
{
	int 	status;

	int i, x, len;
	char buf[IWINFO_BUFSIZE];
	struct iwinfo_scanlist_entry *e;

	// check that iw struct is initialized
	if (!iw) {
		return EXIT_FAILURE;
	}

	// check for scan success
	if (iw->scanlist(ifname, buf, &len))
	{
		printf("Scanning not possible\n\n");
		return EXIT_FAILURE;
	}
	else if (len <= 0)
	{
		printf("No scan results\n\n");
		return EXIT_FAILURE;
	}

	// print the scan results
	for (i = 0, x = 1; i < len; i += sizeof(struct iwinfo_scanlist_entry), x++)
	{
		e = (struct iwinfo_scanlist_entry *) &buf[i];

		printf("network %02d - SSID: %s\n", x, _formatSsid(e->ssid) );
	}


	return status;
}

static char * iwInfoIntf::_formatSsid(char *ssid)
{
	static char buf[IWINFO_ESSID_MAX_SIZE+3];

	if (ssid && ssid[0]) {
		snprintf(buf, sizeof(buf), "\"%s\"", ssid);
	}
	else {
		snprintf(buf, sizeof(buf), "unknown");
	}

	return buf;
}
