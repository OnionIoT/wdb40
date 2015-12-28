#ifndef _IWINFO_INTF_H_
#define _IWINFO_INTF_H_


#include <iwinfo.h>

#define IWINFO_DEVICE_NAME				"wlan0"

class iwInfoIntf : public Module {
public:
	iwInfoIntf(char* device = IWINFO_DEVICE_NAME);
	~iwInfoIntf(void);

	void 	Reset					();

	int 	ReadBackend				();
	int 	PrintScanList			();

private:
	// private functions
	static char* 	_formatSsid	(char* ssid);

	// private members
	char*						wirelessDevice;
	const struct iwinfo_ops 	*iw;
};


#endif // _IWINFO_INTF_H_