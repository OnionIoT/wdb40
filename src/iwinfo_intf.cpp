#include <iwinfo_intf.h>


iwInfoIntf::iwInfoIntf(char* device) 
{
	Reset();

	// initialize the device name
	wirelessDevice 	= new char[IWINFO_MAX_STRING_SIZE];
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

	int 	i, x, len;
	char 	buf[IWINFO_BUFSIZE];
	char	*eSsid;
	struct 	iwinfo_scanlist_entry *e;

	// check that iw struct is initialized
	if (!iw) {
		return EXIT_FAILURE;
	}

	// check for scan success
	if (iw->scanlist(wirelessDevice, buf, &len))
	{
		printf("Scanning not possible\n\n");
		return EXIT_FAILURE;
	}
	else if (len <= 0)
	{
		printf("No scan results\n\n");
		return EXIT_FAILURE;
	}


	// allocate memory for the formatted SSID
	eSsid 	= new char[IWINFO_MAX_STRING_SIZE];

	// print the scan results
	for (i = 0, x = 1; i < len; i += sizeof(struct iwinfo_scanlist_entry), x++)
	{
		e 		= (struct iwinfo_scanlist_entry *) &buf[i];
		_formatSsid(e->ssid, eSsid);

		printf("network %02d - SSID: %s\n", x, eSsid);
	}


	// clean-up
	delete 	eSsid;

	return 	status;
}

void iwInfoIntf::_formatSsid(char *ssid, char *ssidFormatted)
{
	if (ssid && ssid[0]) {
		//snprintf(ssidFormatted, sizeof(ssid), "\"%s\"", ssid);
		sprintf(ssidFormatted, "%s", ssid);
	}
	else {
		//snprintf(ssidFormatted, sizeof(ssid),  "unknown");
		sprintf(ssidFormatted, "hidden");
	}
}
