#ifndef _NETWORK_INFO_H_
#define _NETWORK_INFO_H_

#include <module.h>
#include <wdb40_types.h>

#include <string>


// class to Store all info related to a network
class networkInfo : public Module {
public:
	networkInfo 	(void);
	networkInfo 	(const char* inputSsid, const char* inputEncryptionKey, int inputEncryptionType);
	networkInfo 	(const std::string inputSsid, const std::string inputEncryptionKey, int inputEncryptionType);
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
	void 	SetEncryptionCipher		(int input);
	void 	SetEncryptionSuite		(int input);


	// get functions
	void 	GetSsid					(std::string &output);
	void 	GetBssid				(std::string &output);
	void 	GetEncryptionKey		(std::string &output);

	void 	GetEncryptionType		(int &output);
	void 	GetEncryptionCipher		(int &output);
	void 	GetEncryptionSuite		(int &output);


	// print functions
	void 	PrintBasic				();
	void 	PrintEncryptionKey 		();
	void 	Print 					();


private:
	// private functions
	void 	_formatSsid				(char* ssid, char *ssidFormatted);

	// private members
	std::string		ssid;
	std::string		bssid;
	std::string		encryptionKey;

	int 			encryptionType;
	int 			encryptionCipher;
	int 			encryptionSuite;

	
};





#endif // _NETWORK_INFO_H_

