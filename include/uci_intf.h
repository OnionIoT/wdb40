#ifndef _UCI_INTF_H_
#define _UCI_INTF_H_

extern "C" {
#include "uci.h"
}

#include <wdb40_types.h>
#include <module.h>

#include <network_info.h>


#define UCI_INTF_WIFI_PACKAGE 				"wireless"

#define UCI_INTF_WIFI_IFACE 				"wifi-iface"
#define UCI_INTF_WIFI_IFACE_OPT_SSID 		"ssid"
#define UCI_INTF_WIFI_IFACE_OPT_MODE 		"mode"
#define UCI_INTF_WIFI_IFACE_OPT_NETWORK 	"network"
#define UCI_INTF_WIFI_IFACE_OPT_ENCRYPTION	"encryption"
#define UCI_INTF_WIFI_IFACE_OPT_DISABLED 	"disabled"
#define UCI_INTF_WIFI_IFACE_OPT_KEY 		"key"

#define UCI_INTF_WIFI_IFACE_ENCRYPTION_TYPE_NONE 		"none"
#define UCI_INTF_WIFI_IFACE_ENCRYPTION_TYPE_WEP 		"wep"
#define UCI_INTF_WIFI_IFACE_ENCRYPTION_TYPE_PSK 		"psk"
#define UCI_INTF_WIFI_IFACE_ENCRYPTION_TYPE_PSK2 		"psk2"

#define UCI_INTF_WIFI_IFACE_DISABLED_VALUE_TRUE 		"1"
#define UCI_INTF_WIFI_IFACE_DISABLED_VALUE_FALSE 		"0"

#define UCI_INTF_WIFI_IFACE_MODE_VALUE_AP 				"ap"
#define UCI_INTF_WIFI_IFACE_MODE_VALUE_STA		 		"sta"

// class to perform all iwinfo operations
class uciIntf : public Module {
public:
	uciIntf 	(void);
	~uciIntf 	(void);

	void 	Reset					();

	void 	GetNetworkListSize		(int &output);
	int 	GetNetworkList 			(std::vector<networkInfo> &list);

	int 	ReadBackend 			();
	int 	ReleaseBackend 			();

	int 	ReadWirelessConfig		();
	int 	ProcessConfigData		();

	int 	SetWirelessSectionDisable		(networkInfo *network, int bDisable, int bCommit = 0);
	int 	CommitSectionChanges	();


private:
	// private functions
	int 	_ParseWirelessSection			(struct uci_section *s, const char* sectionName);

	int 	_ParseEncryption 		(const char* input);
	int 	_ParseDisabled			(const char* input);
	int 	_ParseMode				(const char* input);

	int 	_SearchForSection 				(std::string ssid, std::string& configName);


	// private members
	struct uci_context 			*ctx;
	struct uci_ptr 				sectionPtr;

	int 						bBackendInitialized;

	std::vector<networkInfo>	networkList;


};


#endif // _UCI_INTF_H_