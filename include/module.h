#ifndef _MODULE_H_
#define _MODULE_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>


class Module {
public:
	Module(void);
	~Module(void);

	virtual void 	SetVerbosity	(int input);
	void 			SetVerbosity	(bool input);

	void 			_Print 			(int severity, const char* msg, ...);

	void			SetDebugMode	(int input);
	void			SetDebugMode	(bool input);


protected:
	// protected functions
	void 				_SetBit			(int &regVal, int bitNum, int value);
	int 				_GetBit			(int regVal, int bitNum);

	// protected members
	int				verbosityLevel;
	int 			debugLevel;
};

#endif 	// _MODULE_H_