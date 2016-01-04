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



//// functions related to network list
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

//// UCI context functions
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

// free the uci context
int uciIntf::ReleaseBackend()
{
	if (ctx != NULL && bBackendInitialized == 1) {
		bBackendInitialized = 0;
		uci_free_context (ctx);

		return EXIT_SUCCESS;
	}

	return EXIT_FAILURE;
}


//// UCI processing functions
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

// Process the config data
int uciIntf::ProcessConfigData()
{
	int 	status = EXIT_FAILURE;
	
	struct 	uci_element 	*e;
	struct 	uci_section 	*s;

	networkInfo				*network;
	
	// ensure that config has been read
	if (wirelessPtr.target == UCI_TYPE_PACKAGE) {
		
		// look through each section
		uci_foreach_element( &(wirelessPtr.p->sections), e ) {
			if (e->type == UCI_TYPE_SECTION) {
				// grab a pointer to the section
				s = uci_lookup_section(ctx, wirelessPtr.p, e->name);

				// parse the section into a network info object
				status 	= _ParseWirelessSection(s, e->name);
			} // if UCI_TYPE_SECTION

		} // foreach uci element	
		
		status 	=  EXIT_SUCCESS;
	} // if UCI_TYPE_PACKAGE


	return status;
}


//// Parsing functions
// parse config wireless wifi-iface section 
int uciIntf::_ParseWirelessSection(struct uci_section *s, const char* sectionName)
{
	int 	status;

	networkInfo				*network;
	int 					tmp;

	// only look at wifi network setup
	if ( strncmp(s->type, UCI_INTF_WIFI_IFACE, strlen(UCI_INTF_WIFI_IFACE) ) == 0) {
		// lookup the network info 
		/*//o = (ctx, s, UCI_INTF_WIFI_IFACE_OPT_SSID);
		_Print(1, "  section: %s\n", e->name);
		_Print(1, "    ssid:     %s\n", uci_lookup_option_string(ctx, s, UCI_INTF_WIFI_IFACE_OPT_SSID) );
		_Print(1, "    encr:     %s\n", uci_lookup_option_string(ctx, s, UCI_INTF_WIFI_IFACE_OPT_ENCRYPTION) );
		_Print(1, "    disabled: %s\n", uci_lookup_option_string(ctx, s, UCI_INTF_WIFI_IFACE_OPT_DISABLED) );*/


		// experiment with lookup next function
		/*status 	= uci_lookup_next(ctx, &ee, &s->options, UCI_INTF_WIFI_IFACE_OPT_SSID);
		o 		= uci_to_option(ee);
		_Print(1, "    element name: %s, type = %d\n", ee->name, ee->type); */




		//// populate wifiNetwork object
		network 	= new networkInfo( uci_lookup_option_string(ctx, s, UCI_INTF_WIFI_IFACE_OPT_SSID) );
		network->SetVerbosity(1);

		// find the encryption type
		tmp 		= _ParseEncryption(	uci_lookup_option_string(ctx, s, UCI_INTF_WIFI_IFACE_OPT_ENCRYPTION) );
		network->SetEncryptionType(tmp);
		
		// note the wireless section name
		network->SetConfigName(sectionName);
		
		// check if network is disabled
		tmp 		= _ParseDisabled( uci_lookup_option_string(ctx, s, UCI_INTF_WIFI_IFACE_OPT_DISABLED) );
		network->SetDisabled(tmp);

		// find the network mode
		tmp 		= _ParseMode( uci_lookup_option_string(ctx, s, UCI_INTF_WIFI_IFACE_OPT_MODE) );
		network->SetNetworkMode(tmp);


		//// append to vector class member
		networkList.push_back(*network);

		// free the memory
		delete network;

		return EXIT_SUCCESS;
	} // if WIFI_IFACE

	return EXIT_FAILURE;
}

int uciIntf::_ParseEncryption(const char* input)
{
	int output;

	// default is unknown
	output 		= WDB40_ENCRYPTION_UNKNOWN;

	if (strcmp(input, UCI_INTF_WIFI_IFACE_ENCRYPTION_TYPE_NONE) == 0 ) {
		output 	= WDB40_ENCRYPTION_NONE;
	}
	else if (strcmp(input, UCI_INTF_WIFI_IFACE_ENCRYPTION_TYPE_WEP) == 0 ) {
		output 	= WDB40_ENCRYPTION_WEP;
	}
	else if (strcmp(input, UCI_INTF_WIFI_IFACE_ENCRYPTION_TYPE_PSK) == 0 ) {
		output 	= WDB40_ENCRYPTION_PSK;
	}
	else if (strcmp(input, UCI_INTF_WIFI_IFACE_ENCRYPTION_TYPE_PSK2) == 0 ) {
		output 	= WDB40_ENCRYPTION_PSK2;
	}

	return output;
}

int uciIntf::_ParseDisabled(const char* input)
{
	int output;

	// default is disabled
	output 		= WDB40_ENCRYPTION_UNKNOWN;

	if (strcmp(input, UCI_INTF_WIFI_IFACE_DISABLED_VALUE_TRUE) == 0 ) {
		output 	= 1;
	}
	else if (strcmp(input, UCI_INTF_WIFI_IFACE_DISABLED_VALUE_FALSE) == 0 ) {
		output 	= 0;
	}

	return output;
}

int uciIntf::_ParseMode(const char* input)
{
	int output;

	// default is unkown
	output 		= WDB40_NETWORK_MODE_UNKNOWN;

	if (strcmp(input, UCI_INTF_WIFI_IFACE_MODE_VALUE_AP) == 0 ) {
		output 	= WDB40_NETWORK_AP;
	}
	else if (strcmp(input, UCI_INTF_WIFI_IFACE_MODE_VALUE_STA) == 0 ) {
		output 	= WDB40_NETWORK_STA;
	}

	return output;
}




