#include <network_info.h>


networkInfo::networkInfo() 
{
	Reset();
}

networkInfo::networkInfo(const char* inputSsid, const char* inputEncryptionKey, int inputEncryptionType)
{
	Reset();

	ssid 			= std::string(inputSsid);
	encryptionKey 	= std::string(inputEncryptionKey);
	encryptionType 	= inputEncryptionType;
}

networkInfo::networkInfo(const std::string inputSsid, const std::string inputEncryptionKey, int inputEncryptionType)
{
	Reset();

	ssid 			= inputSsid;
	encryptionKey 	= inputEncryptionKey;
	encryptionType 	= inputEncryptionType;
}

networkInfo::~networkInfo() 
{
	// nothing for now
}

void networkInfo::Reset()
{
	ssid 				= "none";
	bssid 				= "none";
	encryptionKey		= "none";

	encryptionType		= WDB40_ENCRYPTION_NONE;
	encryptionCipher	= WDB40_ENCRYPTION_CIPHER_NONE;
	encryptionSuite		= WDB40_ENCRYPTION_SUITE_NONE;
}



//// functions to set class members
void networkInfo::SetSsid(const char* input)
{
	ssid 	= std::string(input);
}

void networkInfo::SetBssid(const char* input)
{
	bssid 	= std::string(input);
}

void networkInfo::SetEncryptionKey(const char* input)
{
	encryptionKey 	= std::string(input);
}


void networkInfo::SetSsid(const std::string input)
{
	ssid 	= input;
}

void networkInfo::SetBssid(const std::string input)
{
	bssid 	= input;
}

void networkInfo::SetEncryptionKey(const std::string input)
{
	encryptionKey 	= input;
}


void networkInfo::SetEncryptionType(int input)
{
	encryptionType 		= input;
}

void networkInfo::SetEncryptionCipher(int input)
{
	encryptionCipher 	= input;
}

void networkInfo::SetEncryptionSuite(int input)
{
	encryptionSuite		= input;
}


//// functions to get class members
void networkInfo::GetSsid (std::string &output)
{
	output 	= ssid;
}

void networkInfo::GetBssid (std::string &output)
{
	output 	= bssid;
}

void networkInfo::GetEncryptionKey (std::string &output)
{
	output 	= encryptionKey;
}


void networkInfo::GetEncryptionType (int &output)
{
	output 	= encryptionType;
}

void networkInfo::GetEncryptionCipher (int &output)
{
	output 	= encryptionCipher;
}

void networkInfo::GetEncryptionSuite (int &output)
{
	output 	= encryptionSuite;
}


//// printing functions
void networkInfo::PrintBasic()
{
	std::string 	tmp0, tmp1;

	_Print(1, "Network:         %s", ssid.c_str() );
	GetEncryptionTypeString(encryptionType, tmp0, tmp1);
	_Print(1, "   Encryption:   %s", tmp1.c_str() );
}

void networkInfo::PrintEncryptionKey() 
{
	_Print(1, "     Key:        %s", encryptionKey.c_str() );
}

void networkInfo::Print()
{
	std::string 	tmp;

	PrintBasic();
	PrintEncryptionKey();

	GetEncryptionCipherString(encryptionCipher, tmp);
	_Print(1, "     Cipher:   %s", tmp.c_str() );

	GetEncryptionSuiteString(encryptionSuite, tmp);
	_Print(1, "     Suite:   %s", tmp.c_str() );

	_Print(1, "   BSSID:        %s", bssid.c_str() );
}


