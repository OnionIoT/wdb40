#ifndef _WDB40_UTILS_H_
#define _WDB40_UTILS_H_


#include <wdb40_types.h>
#include <string>

// functions to return strings from encryption enums
void	GetEncryptionTypeString			(int input, std::string &output);
void	GetEncryptionSubtypeString		(int input, std::string &output);
void	GetEncryptionCipherString		(int input, std::string &output);
void	GetEncryptionSuiteString		(int input, std::string &output);


#endif // _WDB40_UTILS_H_

