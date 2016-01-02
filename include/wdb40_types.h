#ifndef _WDB40_TYPES_H_
#define _WDB40_TYPES_H_


#include <string>

#define IWINFO_MAX_STRING_SIZE			256

// encryption types
typedef enum e_Wdb40NetworkEncryptionType {
	WDB40_ENCRYPTION_NONE				= 0,
	WDB40_ENCRYPTION_WEP_OPEN_SHARED,
	WDB40_ENCRYPTION_WEP_OPEN,
	WDB40_ENCRYPTION_WEP_SHARED_AUTH,
	WDB40_ENCRYPTION_WPA_MIXED,
	WDB40_ENCRYPTION_WPA2,
	WDB40_ENCRYPTION_WPA,
	WDB40_NUM_ENCRYPTION_TYPES
} eWdb40NetworkEncryptionType;

// encryption ciphers
typedef enum e_Wdb40NetworkEncryptionCipher {
	WDB40_ENCRYPTION_CIPHER_NONE		= 0,
	WDB40_ENCRYPTION_CIPHER_WEP_40,
	WDB40_ENCRYPTION_CIPHER_WEP_104,
	WDB40_ENCRYPTION_CIPHER_TKIP,
	WDB40_ENCRYPTION_CIPHER_CCMP,
	WDB40_ENCRYPTION_CIPHER_WRAP,
	WDB40_ENCRYPTION_CIPHER_AES_OCB,
	WDB40_ENCRYPTION_CIPHER_CKIP,
	WDB40_NUM_ENCRYPTION_CIPHERS
} eWdb40NetworkEncryptionCipher;

// encryption suites
typedef enum e_Wdb40NetworkEncryptionSuite {
	WDB40_ENCRYPTION_SUITE_NONE		= 0,
	WDB40_ENCRYPTION_SUITE_PSK,
	WDB40_ENCRYPTION_SUITE_802_1X,
	WDB40_NUM_ENCRYPTION_SUITES
} eWdb40NetworkEncryptionSuite;


// functions to return strings from above enums
void	GetEncryptionTypeString			(int input, std::string &output);
void	GetEncryptionTypeString			(int input, std::string &output, std::string &detailedOutput);
void	GetEncryptionCipherString		(int input, std::string &output);
void	GetEncryptionSuiteString		(int input, std::string &output);


#endif // _WDB40_TYPES_H_

