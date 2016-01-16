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
		bBackendInitialized = 0;
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
	int 	status 	= EXIT_FAILURE;

	struct 	uci_ptr 		ptr;
	struct 	uci_element 	*e;
	struct 	uci_section 	*s;

	char 	*name 	= strdup(UCI_INTF_WIFI_PACKAGE);

	// lookup the wireless config
	if ( uci_lookup_ptr(ctx, &ptr, name, true) == UCI_OK ) {
		// ensure that config has been read
		if (ptr.target == UCI_TYPE_PACKAGE) {
			
			// look through each section
			uci_foreach_element( &(ptr.p->sections), e ) {
				if (e->type == UCI_TYPE_SECTION) {
					// grab a pointer to the section
					s = uci_lookup_section(ctx, ptr.p, e->name);

					// parse the section into a network info object
					status 	= _ParseWirelessSection(s, e->name);
				} // if UCI_TYPE_SECTION

			} // foreach uci element	
		} // if UCI_TYPE_PACKAGE
	}

	// cleanup
	free(name);
	return status;
}

//// UCI action functions
// change the disabled option in a wireless section
int uciIntf::GetWirelessSectionDisable(networkInfo *network, int &bDisable)
{
	int 			status;
	std::string 	configName;

	char 	*wifiSection 	= new char[WDB40_MAX_STRING_SIZE];

	// find the section config name to use
	if ( strcmp(network->GetConfigName().c_str(), NETWORK_INFO_DEFAULT_NONE) != 0) {
		// uci section name is known
		configName 	= network->GetConfigName();
	}
	else {
		// need to find the uci config name
		_Print(2, ">> Searching for section name that matches network ssid\n");
		status 	= _SearchForSection(network->GetSsid(), configName);
	}

	// create the full uci search string
	sprintf(wifiSection, "%s.%s.disabled", UCI_INTF_WIFI_PACKAGE, configName.c_str() );


	// lookup the wireless section
	_Print(2, ">> Reading '%s' disabled state... ", wifiSection);
	if ( uci_lookup_ptr(ctx, &sectionPtr, wifiSection, true) != UCI_OK ) {
		status 	= EXIT_FAILURE;
	}

	// get the disable option
	if (sectionPtr.target == UCI_TYPE_OPTION) {
		bDisable	= _ParseDisabled( sectionPtr.o->v.string );
		_Print(2, "State is '%d'\n", bDisable);

	}

	// clean-up
	delete 	wifiSection;

	return 	status;
}

// change the disabled option in a wireless section
int uciIntf::SetWirelessSectionDisable(networkInfo *network, int bDisable, int bCommit)
{
	int 			status;
	std::string 	configName;

	char 	*wifiSection 	= new char[WDB40_MAX_STRING_SIZE];

	// find the section config name to use
	if ( strcmp(network->GetConfigName().c_str(), NETWORK_INFO_DEFAULT_NONE) != 0) {
		// uci section name is known
		configName 	= network->GetConfigName();
	}
	else {
		// need to find the uci config name
		_Print(2, ">> Searching for section name that matches network ssid\n");
		status 	= _SearchForSection(network->GetSsid(), configName);
	}

	// create the full uci search string
	sprintf(wifiSection, "%s.%s.disabled", UCI_INTF_WIFI_PACKAGE, configName.c_str() );


	// lookup the wireless section
	_Print(2, ">> Changing '%s' state to %d\n", wifiSection, bDisable);
	if ( uci_lookup_ptr(ctx, &sectionPtr, wifiSection, true) != UCI_OK ) {
		status 	= EXIT_FAILURE;
	}

	// set the disable option
	if (sectionPtr.target == UCI_TYPE_OPTION) {
		// set the value
		sectionPtr.value 	= (bDisable == 1 ? "1" : "0");

		status 	= EXIT_SUCCESS;
		
		// set the change
		if ((uci_set(ctx, &sectionPtr) != UCI_OK) || (sectionPtr.o==NULL || sectionPtr.o->v.string==NULL)) {
			_Print(1, "> ERROR: uci set command failed!\n");
			status 	= EXIT_FAILURE;
		}

		// commit the change
		if (bCommit == 1 && status == EXIT_SUCCESS) {
			status 	= CommitSectionChanges();
		}
	}

	// clean-up
	delete 	wifiSection;

	return 	status;
}

// commit the changes in a wireless section
int uciIntf::CommitSectionChanges()
{
	// ensure section pointer actually points to something
	if 	(	!sectionPtr.s &&	//section
			!sectionPtr.p &&	//package
			!sectionPtr.o 		//option
		) 
	{
		_Print(1, "> ERROR: uci ptr has no content\n");
		return EXIT_FAILURE;
	}

	// commit the change
	if (uci_commit(ctx, &sectionPtr.p, false) != UCI_OK){
		_Print(1, "> ERROR: uci commit command failed!\n");
		return EXIT_FAILURE;
	}

	return 	EXIT_SUCCESS;
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
		_Print(3, ">>> Parsing UCI network\n");
		network 	= new networkInfo( uci_lookup_option_string(ctx, s, UCI_INTF_WIFI_IFACE_OPT_SSID) );
		network->SetVerbosity(1);

		// find the encryption type
		_Print(3, "   -- Parsing encryption type\n");
		tmp 		= _ParseEncryption(	uci_lookup_option_string(ctx, s, UCI_INTF_WIFI_IFACE_OPT_ENCRYPTION) );
		network->SetEncryptionType(tmp);
		
		// note the wireless section name
		network->SetConfigName(sectionName);
		
		// check if network is disabled
		_Print(3, "   -- Parsing disabled option\n");
		tmp 		= _ParseDisabled( uci_lookup_option_string(ctx, s, UCI_INTF_WIFI_IFACE_OPT_DISABLED) );
		network->SetDisabled(tmp);

		// find the network mode
		_Print(3, "   -- Parsing mode option\n");
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

// returns enum value for wifi encryption type
int uciIntf::_ParseEncryption(const char* input)
{
	int output;

	// default is unknown
	output 		= WDB40_ENCRYPTION_UNKNOWN;

	// check for valid input
	if (input == NULL) {
		return output;
	}

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

// returns int 0 or 1 based on string input "0" or "1"
int uciIntf::_ParseDisabled(const char* input)
{
	int output;

	// default is enabled (no disabled option means the network is enabled)
	output 		= 0;

	// check for valid input
	if (input == NULL) {
		return output;
	}
	//_Print(3, " --- disable value is '%s'\n", input);

	if (strcmp(input, UCI_INTF_WIFI_IFACE_DISABLED_VALUE_TRUE) == 0 ) {
		output 	= 1;
	}
	else if (strcmp(input, UCI_INTF_WIFI_IFACE_DISABLED_VALUE_FALSE) == 0 ) {
		output 	= 0;
	}

	return output;
}

// returns enum value for wifi network mode (ap or sta)
int uciIntf::_ParseMode(const char* input)
{
	int output;

	// default is unkown
	output 		= WDB40_NETWORK_MODE_UNKNOWN;

	// check for valid input
	if (input == NULL) {
		return output;
	}

	if (strcmp(input, UCI_INTF_WIFI_IFACE_MODE_VALUE_AP) == 0 ) {
		output 	= WDB40_NETWORK_AP;
	}
	else if (strcmp(input, UCI_INTF_WIFI_IFACE_MODE_VALUE_STA) == 0 ) {
		output 	= WDB40_NETWORK_STA;
	}

	return output;
}


// Search for a section based on the ssid name
int uciIntf::_SearchForSection(std::string ssid, std::string& configName)
{
	int 	status = EXIT_FAILURE;
	
	struct 	uci_element 	*e;
	struct 	uci_section 	*s;
	struct 	uci_ptr 		ptr;

	networkInfo				*network;
	
	char 	*name 	= strdup(UCI_INTF_WIFI_PACKAGE);

	// lookup the wireless config
	if ( uci_lookup_ptr(ctx, &ptr, name, true) == UCI_OK ) {
		// ensure that config has been read
		if (ptr.target == UCI_TYPE_PACKAGE) {
			
			// look through each section
			uci_foreach_element( &(ptr.p->sections), e ) {
				if (e->type == UCI_TYPE_SECTION) {
					// grab a pointer to the section
					s = uci_lookup_section(ctx, ptr.p, e->name);

					// only look at wifi network setup
					if ( strncmp(s->type, UCI_INTF_WIFI_IFACE, strlen(UCI_INTF_WIFI_IFACE) ) == 0) {

						// check the ssid
						if ( strcmp(ssid.c_str(), uci_lookup_option_string(ctx, s, UCI_INTF_WIFI_IFACE_OPT_SSID) ) == 0 )
						{	
							_Print(2, "-- found section '%s' with matching ssid '%s'\n", e->name, ssid.c_str() );
							configName 	= std::string(e->name);
							status 	=  EXIT_SUCCESS;
						}
					} // if wifi-iface
				} // if UCI_TYPE_SECTION

			} // foreach uci element	
		} // if UCI_TYPE_PACKAGE
	}

	// cleanup
	free(name);
	return status;
}
