#include <wdb40_utils.h>

void GetEncryptionTypeString (int input, std::string &output)
{
	switch(input) {
		case WDB40_ENCRYPTION_UNKNOWN: 
			output 			= "unknown";
			break;
		case WDB40_ENCRYPTION_WEP: 
			output 			= "wep";
			break;
		case WDB40_ENCRYPTION_PSK2: 
			output 			= "psk2";
			break;
		case WDB40_ENCRYPTION_PSK: 
			output 			= "psk";
			break;
		case WDB40_ENCRYPTION_NONE:
		default:
			output 			= "none";
			break;
	}
}

void GetEncryptionSubtypeString (int input, std::string &output)
{
	switch(input) {
		case WDB40_ENCRYPTION_UNKNOWN: 
			output	= "Unknown";
			break;
		case WDB40_ENCRYPTION_WEP_OPEN_SHARED: 
			output 	= "WEP Mixed (Open/Shared)";
			break;
		case WDB40_ENCRYPTION_WEP_OPEN: 
			output 	= "WEP Open";
			break;
		case WDB40_ENCRYPTION_WEP_SHARED_AUTH: 
			output 	= "WEP Shared";
			break;
		case WDB40_ENCRYPTION_WPA_MIXED: 
			output 	= "WPA Mixed";
			break;
		case WDB40_ENCRYPTION_WPA2: 
			output 	= "WPA2";
			break;
		case WDB40_ENCRYPTION_WPA: 
			output 	= "WPA";
			break;
		case WDB40_ENCRYPTION_NONE:
		default:
			output 	= "None";
			break;
	}
}

void GetEncryptionCipherString (int input, std::string &output)
{
	switch(input) {
		case WDB40_ENCRYPTION_CIPHER_WEP_40: 
			output 			= "WEP-40";
			break;
		case WDB40_ENCRYPTION_CIPHER_WEP_104: 
			output 			= "WEP-104";
			break;
		case WDB40_ENCRYPTION_CIPHER_TKIP: 
			output 			= "TKIP";
			break;
		case WDB40_ENCRYPTION_CIPHER_CCMP: 
			output 			= "CCMP";
			break;
		case WDB40_ENCRYPTION_CIPHER_WRAP: 
			output 			= "WRAP";
			break;
		case WDB40_ENCRYPTION_CIPHER_AES_OCB: 
			output 			= "AES-OCB";
			break;
			
		case WDB40_ENCRYPTION_CIPHER_CKIP: 
			output 			= "CKIP";
			break;
		case WDB40_ENCRYPTION_CIPHER_NONE:
		default:
			output 			= "none";
			break;
	}
}

void GetEncryptionSuiteString (int input, std::string &output)
{
	switch(input) {
		case WDB40_ENCRYPTION_SUITE_PSK: 
			output 			= "PSK";
			break;
			
		case WDB40_ENCRYPTION_SUITE_802_1X: 
			output 			= "801.2X";
			break;
		case WDB40_ENCRYPTION_SUITE_NONE:
		default:
			output 			= "none";
			break;
	}
}
