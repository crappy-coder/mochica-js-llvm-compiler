#ifndef MOENJIN_PLATFORM_WIN_PLATFORM_CONFIGURATION_H
#define MOENJIN_PLATFORM_WIN_PLATFORM_CONFIGURATION_H

//
// define some Windows macros to keep things as
// light as possible, we shouldn't need to use
// to much from the Windows API
//
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif//WIN32_LEAN_AND_MEAN

#ifndef NOMINMAX
#define NOMINMAX
#endif//NOMINMAX

#ifndef NOKERNEL
#define NOKERNEL
#endif//NOKERNEL

#ifndef NOUSER
#define NOUSER
#endif//NOUSER

#ifndef NOSERVICE
#define NOSERVICE
#endif//NOSERVICE

#ifndef NOSOUND
#define NOSOUND
#endif//NOSOUND

#ifndef NOGDI
#define NOGDI
#endif//NOGDI

#ifndef NOMCX
#define NOMCX
#endif//NOMCX

#ifndef NONLS
#define NONLS
#endif//NONLS

#ifndef NOCOMM
#define NOCOMM
#endif//NOCOMM

#ifndef NOHELP
#define NOHELP
#endif//NOHELP

// Require Windows Vista or newer
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif//_WIN32_WINNT

#ifndef WINVER
#define WINVER 0x0600
#endif//WINVER



#include <Windows.h>


//
// undefine some common names that WE would like to possibly use
//
#ifdef VOID
#undef VOID
#endif

#ifdef DELETE
#undef DELETE
#endif

#ifdef IN
#undef IN
#endif

#ifdef THIS
#undef THIS
#endif

#ifdef CONST
#undef CONST
#endif

#ifdef NAN
#undef NAN
#endif

#ifdef GetObject
#undef GetObject
#endif

#ifdef GetClassName
#undef GetClassName 
#endif

#ifdef snprintf
#undef snprintf
#endif

#define snprintf sprintf_s

#endif