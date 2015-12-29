#ifndef _IWINFO_INTF_H_
#define _IWINFO_INTF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "iwinfo.h"

#ifdef __cplusplus
}
#endif

#include <module.h>


#define IWINFO_DEVICE_NAME				"wlan0"
#define IWINFO_MAX_STRING_SIZE			256



class iwInfoIntf : public Module {
public:
	iwInfoIntf(char* device = IWINFO_DEVICE_NAME);
	~iwInfoIntf(void);

	void 	Reset					();

	int 	ReadBackend				();
	int 	PrintScanList			();

private:
	// private functions
	void 	_formatSsid				(char* ssid, char *ssidFormatted);

	// private members
	char*						wirelessDevice;
	const struct iwinfo_ops 	*iw;
};


#endif // _IWINFO_INTF_H_