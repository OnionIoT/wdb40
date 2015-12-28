#include <module.h>

Module::Module(void)
{
	// not verbose by default
	verbosityLevel	= 0;

	// not in debug mode by default
	debugLevel		= 0;
}

Module::~Module(void)
{
	// nothing for now
}


// Debug Functions
void Module::SetVerbosity (int input)
{
	verbosityLevel	= input;
}

void Module::SetVerbosity (bool input)
{
	verbosityLevel	= (input ? 1 : 0);
}

void Module::SetDebugMode (int input)
{
	debugLevel		= input;
}

void Module::SetDebugMode (bool input)
{
	debugLevel		= (input ? 1 : 0);
}

void Module::_Print (int severity, const char* msg, ...)
{
	va_list 	argptr;	

	if (verbosityLevel >= severity) {
		va_start(argptr, msg);
		vprintf(msg, argptr);
		va_end(argptr);
	}
}

//// Register access
// change the value of a single bit
void Module::_SetBit(int &regVal, int bitNum, int value)
{
	regVal ^= (-value ^ regVal) & (1 << bitNum);
}

// find the value of a single bit
int Module::_GetBit(int regVal, int bitNum)
{
	int value;

	// isolate the specific bit
	value = ((regVal >> bitNum) & 0x1);

	return (value);
}
