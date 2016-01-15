#ifndef _WDB40_TOOL_H_
#define _WDB40_TOOL_H_


#include <iostream>
#include <fstream>

#include <wdb40_types.h>
#include <wdb40_utils.h>
#include <module.h>

#include <network_info.h>
#include <iwinfo_intf.h>
#include <uci_intf.h>
#include <ubus_intf.h>


#define WDB40_TOOL_TIMEOUT_COUNT				300
#define WDB40_TOOL_TIMEOUT_DEFAULT_SECONDS 		10

#define WDB40_TOOL_FILE_PATH			"/tmp"
#define WDB40_TOOL_FILE_CONFIG 			"wifi_config"
#define WDB40_TOOL_FILE_SCAN 			"wifi_scan"
#define WDB40_TOOL_FILE_MATCH 			"wifi_match"



// class to Store all info related to a network
class wdb40Tool : public Module {
public:
	wdb40Tool 	(void);

	~wdb40Tool 	(void);

	void 	Reset						();

	// uci intf functions	
	int 	ReadConfigNetworks			(int bPrintToFile = 0);

	int 	SetApWirelessEnable 		(int bEnable);
	int 	SetAllStaWirelessEnable		(int bEnable);

	int 	EnableMatchedNetwork 		();

	// iwinfo intf functions
	int 	ScanAvailableNetworks		();

	// ubus intf functions
	int 	CheckWirelessStatus 		(int &bUp);
	int 	WaitUntilWirelessStatus	 	(int bUp, int timeoutSeconds = WDB40_TOOL_TIMEOUT_DEFAULT_SECONDS);

	// wdb40 functions
	int 	CheckForConfigNetworks		(int bPrintToFile = 0);
	int 	RestartWireless 			();

	void 	PrintMatchNetworks 			();	

	// file functions
	int 	FetchConfigNetworks 		();
	int 	FetchScanNetworks			();
	int 	FetchMatchNetworks			();


private:
	// private functions
	void 	_PrintNetworkList				(std::vector<networkInfo> &networkList);

	void 	_FilePrintNetworkList			(std::vector<networkInfo> &networkList, std::string filename, std::string path = WDB40_TOOL_FILE_PATH);
	int 	_FileReadNetworkList			(std::vector<networkInfo> &networkList, std::string filename, std::string path = WDB40_TOOL_FILE_PATH);

	int 	_CompareNetworks				(networkInfo network1, networkInfo network2, int &bMatch);

	int 	_GenericNetworkListTraversal	(int mode, int param0);

	void 	_AddNetwork 					(std::vector<networkInfo> &networkList, std::string ssid, int encrType);


	// private members
	iwInfoIntf	*iw;
	uciIntf 	*uci;
	ubusIntf 	*ubus;

	std::vector<networkInfo> 	scanList;
	std::vector<networkInfo> 	configList;
	std::vector<networkInfo> 	matchList;

	typedef enum e_Wdb40ToolNetworkListTraversalMode {
		WDB40_TOOL_TRAVERSAL_ENABLE_AP 	= 0,
		WDB40_TOOL_TRAVERSAL_ENABLE_ALL_STA,
		WDB40_TOOL_NUM_TRAVERSAL_MODES
	} eWdb40ToolNetworkListTraversalMode;
};





#endif // _WDB40_TOOL_H_

