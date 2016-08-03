#ifndef _IWINFO_INTF_H_
#define _IWINFO_INTF_H_

extern "C" {
#include "iwinfo.h"
}


#include <wdb40_types.h>
#include <module.h>

#include <network_info.h>

#include <vector>



// class to perform all iwinfo operations
class iwInfoIntf : public Module {
public:
	iwInfoIntf();
	iwInfoIntf(std::string device);
	iwInfoIntf(char* device);
	~iwInfoIntf(void);

	void 	Reset					();
	void 	ReleaseBackend 			();

	int 	ReadBackend				();
	int 	ProcessScanList			();

	int 	WifiScan 				();

	void 	GetScanListSize			(int &output);
	int 	GetScanList 			(std::vector<networkInfo> &list);

	void 	PrintScanList			();

private:
	// private functions
	void 	_formatSsid					(char* ssid, char *ssidFormatted);
	void 	_formatEncryptionType		(struct iwinfo_crypto_entry *c, int &encryptionType, int &encryptionSubtype);

	// private members
	char*						wirelessDevice;
	const struct iwinfo_ops 	*iw;
	int 						bBackendInitialized;

	std::vector<networkInfo>	networkList;
};


#endif // _IWINFO_INTF_H_