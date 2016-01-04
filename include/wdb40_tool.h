#ifndef _WDB40_TOOL_H_
#define _WDB40_TOOL_H_

#include <wdb40_types.h>
#include <wdb40_utils.h>
#include <module.h>

#include <network_info.h>
#include <iwinfo_intf.h>
#include <uci_intf.h>


// class to Store all info related to a network
class wdb40Tool : public Module {
public:
	wdb40Tool 	(void);

	~wdb40Tool 	(void);

	void 	Reset						();

	int 	ScanAvailableNetworks		();
	int 	ReadConfigNetworks			();


private:
	// private functions
	

	// private members
	iwInfoIntf	*iw;
	uciIntf 	*uci;

	std::vector<networkInfo> 	scanList;
	std::vector<networkInfo> 	configList;

	
};





#endif // _WDB40_TOOL_H_

