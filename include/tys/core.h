#pragma once

#ifndef TYS_HEADER_INCLUDE
#define TYS_HEADER_INCLUDE

//	This file should include the BARE minimum for the rest
//	Anything that might not be necessary everywhere else shouldn't go here

//	Allows a env.h to be used to change the parameters of the library
#if __has_include("env.h")
#include "env.h"
#elif __has_include(<env.h>)
#include <env.h>
#endif

#endif	//	Include
