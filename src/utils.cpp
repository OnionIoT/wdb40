#include <wdb40_types.h>

void GetEncryptionTypeString (int input, std::string &output)
{
	switch(input) {
		case WDB40_ENCRYPTION_WEP_OPEN_SHARED: 
			output 			= "wep";
			break;
		case WDB40_ENCRYPTION_WEP_OPEN: 
			output 			= "wep";
			break;
		case WDB40_ENCRYPTION_WEP_SHARED_AUTH: 
			output 			= "wep";
			break;
		case WDB40_ENCRYPTION_WPA_MIXED: 
			output 			= "psk2";
			break;
		case WDB40_ENCRYPTION_WPA2: 
			output 			= "psk2";
			break;
		case WDB40_ENCRYPTION_WPA: 
			output 			= "psk";
			break;
		case WDB40_ENCRYPTION_NONE:
		default:
			output 			= "none";
			break;
	}
}

void GetEncryptionTypeString (int input, std::string &output, std::string &detailedOutput)
{
	GetEncryptionTypeString(input, output);

	switch(input) {
		case WDB40_ENCRYPTION_WEP_OPEN_SHARED: 
			detailedOutput 	= "WEP Open/Shared";
			break;
		case WDB40_ENCRYPTION_WEP_OPEN: 
			detailedOutput 	= "WEP Open";
			break;
		case WDB40_ENCRYPTION_WEP_SHARED_AUTH: 
			detailedOutput 	= "WEP Shared";
			break;
		case WDB40_ENCRYPTION_WPA_MIXED: 
			detailedOutput 	= "WPA Mixed";
			break;
		case WDB40_ENCRYPTION_WPA2: 
			detailedOutput 	= "WPA2";
			break;
		case WDB40_ENCRYPTION_WPA: 
			detailedOutput 	= "WPA";
			break;
		case WDB40_ENCRYPTION_NONE:
		default:
			detailedOutput 	= "None";
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
