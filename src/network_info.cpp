#include <network_info.h>


networkInfo::networkInfo() 
{
	Reset();
}

// constructor with SSID, encryption type, and encryption key
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

// constructor with SSID and encryption type
networkInfo::networkInfo(const char* inputSsid, int inputEncryptionType)
{
	Reset();

	ssid 			= std::string(inputSsid);
	encryptionType 	= inputEncryptionType;
}

networkInfo::networkInfo(const std::string inputSsid, int inputEncryptionType)
{
	Reset();

	ssid 			= inputSsid;
	encryptionType 	= inputEncryptionType;
}

// destructor
networkInfo::~networkInfo() 
{
	// nothing for now
}

void networkInfo::Reset()
{
	ssid 				= NETWORK_INFO_DEFAULT_NONE;
	bssid 				= NETWORK_INFO_DEFAULT_NONE;
	encryptionKey		= NETWORK_INFO_DEFAULT_NONE;
	cfgName				= NETWORK_INFO_DEFAULT_NONE;

	encryptionType		= WDB40_ENCRYPTION_NONE;
	encryptionCipher	= WDB40_ENCRYPTION_CIPHER_NONE;
	encryptionSuite		= WDB40_ENCRYPTION_SUITE_NONE;

	bDisabled 			= 0;
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

void networkInfo::SetConfigName(const char* input)
{
	cfgName 	= std::string(input);
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

void networkInfo::SetConfigName(const std::string input)
{
	cfgName 	= input;
}


void networkInfo::SetEncryptionType(int input)
{
	encryptionType 		= input;
}

void networkInfo::SetEncryptionSubtype(int input)
{
	encryptionSubtype 	= input;
}

void networkInfo::SetEncryptionCipher(int input)
{
	encryptionCipher 	= input;
}

void networkInfo::SetEncryptionSuite(int input)
{
	encryptionSuite		= input;
}

void networkInfo::SetDisabled(int input)
{
	bDisabled			= input;
}


//// functions to get class members 
// through argument
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

void networkInfo::GetConfigName (std::string &output)
{
	output 	= cfgName;
}


void networkInfo::GetEncryptionType (int &output)
{
	output 	= encryptionType;
}

void networkInfo::GetEncryptionSubtype (int &output)
{
	output 	= encryptionSubtype;
}

void networkInfo::GetEncryptionCipher (int &output)
{
	output 	= encryptionCipher;
}

void networkInfo::GetEncryptionSuite (int &output)
{
	output 	= encryptionSuite;
}

void networkInfo::GetDisabled (int &output)
{
	output 	= bDisabled;
}


// as return value
std::string networkInfo::GetSsid ()
{
	return (ssid);
}

std::string networkInfo::GetBssid ()
{
	return (bssid);
}

std::string networkInfo::GetEncryptionKey ()
{
	return (encryptionKey);
}

std::string networkInfo::GetConfigName ()
{
	return (cfgName);
}


int networkInfo::GetEncryptionType ()
{
	return (encryptionType);
}

int networkInfo::GetEncryptionSubtype ()
{
	return (encryptionSubtype);
}

int networkInfo::GetEncryptionCipher ()
{
	return (encryptionCipher);
}

int networkInfo::GetEncryptionSuite ()
{
	return (encryptionSuite);
}

int networkInfo::GetDisabled ()
{
	return (bDisabled);
}


//// printing functions
void networkInfo::PrintBasic()
{
	std::string 	encrType, encrSubtype;

	GetEncryptionTypeString(encryptionType, encrType);
	GetEncryptionSubtypeString(encryptionSubtype, encrSubtype);

	_Print(1, "Network:       %s\n", ssid.c_str() );
	_Print(1, "   Encryption:   %s (%s)\n", encrType.c_str(), encrSubtype.c_str() );
}

void networkInfo::PrintEncryptionKey() 
{
	_Print(1, "     Key:        %s\n", encryptionKey.c_str() );
}

void networkInfo::Print()
{
	std::string 	tmp;

	PrintBasic();
	PrintEncryptionKey();

	GetEncryptionCipherString(encryptionCipher, tmp);
	_Print(1, "     Cipher:     %s\n", tmp.c_str() );

	GetEncryptionSuiteString(encryptionSuite, tmp);
	_Print(1, "     Suite:      %s\n", tmp.c_str() );

	_Print(1, "   BSSID:        %s\n", bssid.c_str() );
	_Print(1, "   UCI cfg:      %s\n", cfgName.c_str() );
}






