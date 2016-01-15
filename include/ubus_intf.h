#ifndef _UBUS_INTF_H_
#define _UBUS_INTF_H_

extern "C" {
#include "libubus.h"
}

#include <wdb40_types.h>
#include <module.h>

#include <string>



#define UBUS_INTF_DEFAULT_TIMEOUT 			30
#define UBUS_INTF_TIMEOUT_MULTIPLIER 		1000

#define UBUS_INTF_NETWORK_WIRELESS_PATH 			"network.wireless"
#define UBUS_INTF_NETWORK_INTF_WWAN_PATH 			"network.interface.wwan"

#define UBUS_INTF_NETWORK_METHOD_STATUS 					"status"
#define UBUS_INTF_NETWORK_METHOD_STATUS_ATTRIBUTE_UP 		"up"

#define UBUS_INTF_VALUE_TRUE 						"1"
#define UBUS_INTF_VALUE_FALSE 						"0"


// class to perform all ubus operations
class ubusIntf : public Module {
public:
	ubusIntf 	(void);
	~ubusIntf 	(void);

	void 	Reset						();
	
	int 	ReadContext 				();
	int 	ReleaseContext 				();

	int 	GetNetworkUpStatus			(int &bUp, int statusType);

	// static members and functions
	static struct blob_attr 			*receivedAttribute;
	static char*	 					attributeName;
	static char*	 					attributeValue;
	static int 		 					attributeType;


private:
	// private functions
	int 	_ReadBooleanValue 			(int &bVal);


	// private members
	struct 	ubus_context 		*ctx;
	struct 	blob_buf 			bMsg;
	
	char 						*ubus_socket;
	int 						bBackendInitialized;
	int 						timeout;

};


#endif // _UBUS_INTF_H_