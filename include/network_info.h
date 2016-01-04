#ifndef _NETWORK_INFO_H_
#define _NETWORK_INFO_H_

#include <wdb40_types.h>
#include <wdb40_utils.h>
#include <module.h>

#include <string>


#define NETWORK_INFO_DEFAULT_NONE		"default-none"


// class to Store all info related to a network
class networkInfo : public Module {
public:
	networkInfo 	(void);
	
	networkInfo 	(const char* inputSsid, const char* inputEncryptionKey, int inputEncryptionType);
	networkInfo 	(const std::string inputSsid, const std::string inputEncryptionKey, int inputEncryptionType);

	networkInfo 	(const char* inputSsid, int inputEncryptionType);
	networkInfo 	(const std::string inputSsid, int inputEncryptionType);

	~networkInfo 	(void);

	void 	Reset					();


	// set functions
	void 	SetSsid					(const char* input);
	void 	SetBssid				(const char* input);
	void 	SetEncryptionKey		(const char* input);

	void 	SetSsid					(const std::string input);
	void 	SetBssid				(const std::string input);
	void 	SetEncryptionKey		(const std::string input);

	void 	SetEncryptionType		(int input);
	void 	SetEncryptionSubtype	(int input);
	void 	SetEncryptionCipher		(int input);
	void 	SetEncryptionSuite		(int input);

	void 	SetDisabled				(int input);


	// get functions
	void 			GetSsid					(std::string &output);
	void 			GetBssid				(std::string &output);
	void 			GetEncryptionKey		(std::string &output);
		
	void 			GetEncryptionType		(int &output);
	void 			GetEncryptionSubtype	(int &output);
	void 			GetEncryptionCipher		(int &output);
	void 			GetEncryptionSuite		(int &output);
		
	void 			GetDisabled 			(int &output);


	std::string 	GetSsid					();
	std::string 	GetBssid				();
	std::string 	GetEncryptionKey		();

	int 		 	GetEncryptionType		();
	int 		 	GetEncryptionSubtype	();
	int 		 	GetEncryptionCipher		();
	int 		 	GetEncryptionSuite		();

	int 		 	GetDisabled 			();



	// print functions
	void 			PrintBasic				();
	void 			PrintEncryptionKey 		();
	void 			Print 					();


private:
	// private functions
	void 	_formatSsid				(char* ssid, char *ssidFormatted);
	void 	_PrintNetworkList		(std::vector<networkInfo> networkList);

	// private members
	std::string		ssid;
	std::string		bssid;
	std::string		encryptionKey;

	int 			encryptionType;
	int 			encryptionSubtype;
	int 			encryptionCipher;
	int 			encryptionSuite;

	int 			bDisabled;

	
};





#endif // _NETWORK_INFO_H_

