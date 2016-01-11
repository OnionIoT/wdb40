#ifndef _WDB40_TOOL_H_
#define _WDB40_TOOL_H_

#include <wdb40_types.h>
#include <wdb40_utils.h>
#include <module.h>

#include <network_info.h>
#include <iwinfo_intf.h>
#include <uci_intf.h>
#include <ubus_intf.h>


#define WDB40_TOOL_TIMEOUT 				3000;


// class to Store all info related to a network
class wdb40Tool : public Module {
public:
	wdb40Tool 	(void);

	~wdb40Tool 	(void);

	void 	Reset						();

	// uci intf functions	
	int 	ReadConfigNetworks			();

	int 	SetApWirelessEnable 		(int bEnable);
	int 	SetAllStaWirelessEnable		(int bEnable);

	// iwinfo intf functions
	int 	ScanAvailableNetworks		();

	// ubus intf functions
	int 	CheckWirelessStatus 		(int &bUp);
	int 	WaitUntilWirelessStatus	 	(int bUp);

	// wdb40 functions
	int 	CheckForConfigNetworks		();
	int 	RestartWireless 			();


private:
	// private functions
	void 	_PrintNetworkList				(std::vector<networkInfo> networkList);
	int 	_CompareNetworks				(networkInfo network1, networkInfo network2, int &bMatch);

	int 	_GenericNetworkListTraversal	(int mode, int param0);


	// private members
	iwInfoIntf	*iw;
	uciIntf 	*uci;
	ubusIntf 	*ubus;

	std::vector<networkInfo> 	scanList;
	std::vector<networkInfo> 	configList;

	typedef enum e_Wdb40ToolNetworkListTraversalMode {
		WDB40_TOOL_TRAVERSAL_ENABLE_AP 	= 0,
		WDB40_TOOL_TRAVERSAL_ENABLE_ALL_STA,
		WDB40_TOOL_NUM_TRAVERSAL_MODES
	} eWdb40ToolNetworkListTraversalMode;
};





#endif // _WDB40_TOOL_H_

