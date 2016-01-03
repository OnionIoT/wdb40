#ifndef _UCI_INTF_H_
#define _UCI_INTF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "uci.h"

#ifdef __cplusplus
}
#endif

#include <wdb40_types.h>
#include <module.h>

#include <network_info.h>


#define UCI_INTF_WIFI_IFACE 				"wifi-iface"
#define UCI_INTF_WIFI_IFACE_OPT_SSID 		"ssid"
#define UCI_INTF_WIFI_IFACE_OPT_MODE 		"mode"
#define UCI_INTF_WIFI_IFACE_OPT_NETWORK 	"network"
#define UCI_INTF_WIFI_IFACE_OPT_ENCRYPTION	"encryption"
#define UCI_INTF_WIFI_IFACE_OPT_DISABLED 	"disabled"
#define UCI_INTF_WIFI_IFACE_OPT_KEY 		"key"

// class to perform all iwinfo operations
class uciIntf : public Module {
public:
	uciIntf 	(void);
	~uciIntf 	(void);

	void 	Reset					();

	void 	ReleaseBackend 			();

	int 	ReadBackend 			();
	int 	ReadWirelessConfig		();
	int 	ProcessConfigData		();



private:
	// private functions
	

	// private members
	struct uci_context 			*ctx;
	struct uci_ptr 				wirelessPtr;

	int 						bBackendInitialized;
};


#endif // _UCI_INTF_H_