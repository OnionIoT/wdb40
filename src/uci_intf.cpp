#include <uci_intf.h>


uciIntf::uciIntf() 
{
	Reset();

	// verbosity setting
	SetVerbosity(1);
}

uciIntf::~uciIntf() 
{
	ReleaseBackend();
}


void uciIntf::Reset()
{
	ctx 			= NULL;

	bBackendInitialized 	= 0;
}

// free the uci context
void uciIntf::ReleaseBackend()
{
	if (ctx != NULL && bBackendInitialized == 1) {
		bBackendInitialized = 0;
		uci_free_context (ctx);
	}
}


// Allocate the UCI context
int uciIntf::ReadBackend()
{
	int status;

	// initialization of uci context
	ctx 	= uci_alloc_context();

	if (!ctx) {
		return EXIT_FAILURE;
	}

	bBackendInitialized = 1;
	return EXIT_SUCCESS;
}

// Read the UCI wireless configuration
int uciIntf::ReadWirelessConfig()
{
	int 	status;

	char *config 	= strdup("wireless");

	// lookup the wireless config
	if ( uci_lookup_ptr(ctx, &wirelessPtr, config, true) != UCI_OK ) {
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int uciIntf::ProcessConfigData()
{
	int 	status = EXIT_FAILURE;
	
	struct 	uci_element 	*e;
	struct 	uci_section 	*s;
	struct 	uci_option 		*o;

	networkInfo				*network;
	int 					encr;
	
	// ensure that config has been read
	if (wirelessPtr.target == UCI_TYPE_PACKAGE) {
		
		// look through each section
		uci_foreach_element( &(wirelessPtr.p->sections), e ) {
			if (e->type == UCI_TYPE_SECTION) {
				// grab a pointer to the section
				s = uci_lookup_section(ctx, wirelessPtr.p, e->name);

				// only look at wifi network setup
				if ( strncmp(s->type, UCI_INTF_WIFI_IFACE, strlen(UCI_INTF_WIFI_IFACE) ) == 0) {
					// lookup the network info 
					//o = (ctx, s, UCI_INTF_WIFI_IFACE_OPT_SSID);
					/*_Print(1, "  section: %s\n", e->name);
					_Print(1, "    ssid:     %s\n", uci_lookup_option_string(ctx, s, UCI_INTF_WIFI_IFACE_OPT_SSID) );
					_Print(1, "    encr:     %s\n", uci_lookup_option_string(ctx, s, UCI_INTF_WIFI_IFACE_OPT_ENCRYPTION) );
					_Print(1, "    disabled: %s\n", uci_lookup_option_string(ctx, s, UCI_INTF_WIFI_IFACE_OPT_DISABLED) );*/

					// populate wifiNetwork object
					_formatEncryption(uci_lookup_option_string(ctx, s, UCI_INTF_WIFI_IFACE_OPT_ENCRYPTION), encr);
					network 	= new networkInfo(uci_lookup_option_string(ctx, s, UCI_INTF_WIFI_IFACE_OPT_SSID), encr);
					network->SetConfigName(e->name);
					network->SetVerbosity(1);

					// append to vector class member
					networkList.push_back(*network);

					// free the memory
					delete network;
				} // if WIFI_IFACE
			} // if UCI_TYPE_SECTION

		} // foreach uci element	
		
		status 	=  EXIT_SUCCESS;
	} // if UCI_TYPE_PACKAGE

	

	return status;
}

void uciIntf::_formatEncryption(const char* input, int &encryptionType)
{
	// default is unknown
	encryptionType 		= WDB40_ENCRYPTION_UNKNOWN;

	if (strcmp(input, UCI_INTF_WIFI_IFACE_ENCRYPTION_TYPE_NONE) == 0 ) {
		encryptionType 	= WDB40_ENCRYPTION_NONE;
	}
	else if (strcmp(input, UCI_INTF_WIFI_IFACE_ENCRYPTION_TYPE_WEP) == 0 ) {
		encryptionType 	= WDB40_ENCRYPTION_WEP;
	}
	else if (strcmp(input, UCI_INTF_WIFI_IFACE_ENCRYPTION_TYPE_PSK) == 0 ) {
		encryptionType 	= WDB40_ENCRYPTION_PSK;
	}
	else if (strcmp(input, UCI_INTF_WIFI_IFACE_ENCRYPTION_TYPE_PSK2) == 0 ) {
		encryptionType 	= WDB40_ENCRYPTION_PSK2;
	}
}


void uciIntf::GetNetworkListSize(int &output)
{
	output 	= networkList.size();
}

int uciIntf::GetNetworkList(std::vector<networkInfo> &list)
{
	if (networkList.size() > 0) {
		list 	= networkList;

		return EXIT_SUCCESS;
	}

	return EXIT_FAILURE;
}



