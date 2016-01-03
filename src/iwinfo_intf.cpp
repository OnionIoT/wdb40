#include <iwinfo_intf.h>


iwInfoIntf::iwInfoIntf(char* device) 
{
	Reset();

	// initialize the device name
	wirelessDevice 	= new char[IWINFO_MAX_STRING_SIZE];
	strncpy(wirelessDevice, device, strlen(device) );


	// verbosity setting
	SetVerbosity(1);
}

iwInfoIntf::~iwInfoIntf() 
{
	// free dynamically allocated memory
	delete 	wirelessDevice;

	// iwinfo cleanup
	ReleaseBackend();
}


void iwInfoIntf::Reset()
{
	wirelessDevice 		= NULL;
	bBackendInitialized 	= 0;
}

void iwInfoIntf::ReleaseBackend()
{
	// iwinfo cleanup
	if (bBackendInitialized) {
		iwinfo_finish();
		bBackendInitialized = 0;
	}
}


// function to connect to iwinfo backend
int iwInfoIntf::ReadBackend()
{
	// initialize the iw struct
	iw = iwinfo_backend(wirelessDevice);

	if (!iw)
	{
		// iw init failed
		return EXIT_FAILURE;
	}

	// iw init success
	bBackendInitialized = 1;
	return EXIT_SUCCESS;
}

// function to process the iwinfo scan list
int iwInfoIntf::ProcessScanList()
{
	int 	i, x, len, tmpInt;
	char 	buf[IWINFO_BUFSIZE];
	char	*tmpChar;

	struct 	iwinfo_scanlist_entry 	*e;
	networkInfo						*network;

	// check that iw struct is initialized
	if (!iw) {
		return EXIT_FAILURE;
	}

	// check for scan success
	if (iw->scanlist(wirelessDevice, buf, &len))
	{
		_Print(0, "Scanning not possible\n\n");
		return EXIT_FAILURE;
	}
	else if (len <= 0)
	{
		_Print(0, "No scan results\n\n");
		return EXIT_FAILURE;
	}


	// allocate memory for the char*
	tmpChar 	= new char[IWINFO_MAX_STRING_SIZE];

	// print the scan results
	for (i = 0, x = 1; i < len; i += sizeof(struct iwinfo_scanlist_entry), x++)
	{
		e 	= (struct iwinfo_scanlist_entry *) &buf[i];
		_formatSsid(e->ssid, tmpChar);
		_formatEncryptionType(&e->crypto, tmpInt);

		// populate wifiNetwork class
		network 	= new networkInfo(tmpChar, tmpInt);
		network->SetVerbosity(1);

		// append to vector class member
		networkList.push_back(*network);

		// free the memory
		delete network;
	}


	// clean-up
	delete 	tmpChar;

	return 	EXIT_SUCCESS;
}

// perform all steps required for wifi scan
//	* initialize the backend
//	* process the scanned info
// 	* disable the backend
int iwInfoIntf::WifiScan()
{
	int 	status;

	// initialize the backend and process the scan info
	status	=  ReadBackend();
	status	|= ProcessScanList();

	// free the backend
	ReleaseBackend();

	return (status);
}

void iwInfoIntf::GetScanListSize(int &output)
{
	output 	= networkList.size();
}

void iwInfoIntf::PrintScanList()
{
	for (std::vector<networkInfo>::iterator it = networkList.begin(); it != networkList.end(); it++) {
		(*it).PrintBasic();
	}
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

void iwInfoIntf::_formatEncryptionType(struct iwinfo_crypto_entry *c, int &encryptionType)
{
	// check if struct is properly initialized
	if (!c) {
		encryptionType 	= WDB40_ENCRYPTION_UNKNOWN;
		return;
	}

	// default is none
	encryptionType 		= WDB40_ENCRYPTION_NONE;

	// check for encryption
	if (c->enabled)
	{
		/* WEP */
		if (c->auth_algs && !c->wpa_version)
		{
			if ((c->auth_algs & IWINFO_AUTH_OPEN) &&
				(c->auth_algs & IWINFO_AUTH_SHARED))
			{
				encryptionType 		= WDB40_ENCRYPTION_WEP_OPEN_SHARED;
			}
			else if (c->auth_algs & IWINFO_AUTH_OPEN)
			{
				encryptionType 		= WDB40_ENCRYPTION_WEP_OPEN;
			}
			else if (c->auth_algs & IWINFO_AUTH_SHARED)
			{
				encryptionType 		= WDB40_ENCRYPTION_WEP_SHARED_AUTH;
			}
		}

		/* WPA */
		else if (c->wpa_version)
		{
			switch (c->wpa_version) {
				case 3:
					encryptionType 		= WDB40_ENCRYPTION_WPA_MIXED;
					break;

				case 2:
					encryptionType 		= WDB40_ENCRYPTION_WPA2;
					break;

				case 1:
					encryptionType 		= WDB40_ENCRYPTION_WPA;
					break;
			}
		}

	}

}
