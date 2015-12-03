#ifndef MOENJIN_PLATFORM_CONFIGURATION_H
#define MOENJIN_PLATFORM_CONFIGURATION_H

/**********************************************************************************************************************************/                                                                                                                          
/*                                                                                                                            	 */
/*    MMMMMMMM               MMMMMMMM                 EEEEEEEEEEEEEEEEEEEEEE                   jjjj   iiii                   	 */
/*    M:::::::M             M:::::::M                 E::::::::::::::::::::E                  j::::j i::::i                  	 */
/*    M::::::::M           M::::::::M                 E::::::::::::::::::::E                   jjjj   iiii                   	 */
/*    M:::::::::M         M:::::::::M                 EE::::::EEEEEEEEE::::E                                                 	 */
/*    M::::::::::M       M::::::::::M   ooooooooooo     E:::::E       EEEEEEnnnn  nnnnnnnn   jjjjjjjiiiiiiinnnn  nnnnnnnn    	 */
/*    M:::::::::::M     M:::::::::::M oo:::::::::::oo   E:::::E             n:::nn::::::::nn j:::::ji:::::in:::nn::::::::nn  	 */
/*    M:::::::M::::M   M::::M:::::::Mo:::::::::::::::o  E::::::EEEEEEEEEE   n::::::::::::::nn j::::j i::::in::::::::::::::nn 	 */
/*    M::::::M M::::M M::::M M::::::Mo:::::ooooo:::::o  E:::::::::::::::E   nn:::::::::::::::nj::::j i::::inn:::::::::::::::n	 */
/*    M::::::M  M::::M::::M  M::::::Mo::::o     o::::o  E:::::::::::::::E     n:::::nnnn:::::nj::::j i::::i  n:::::nnnn:::::n	 */
/*    M::::::M   M:::::::M   M::::::Mo::::o     o::::o  E::::::EEEEEEEEEE     n::::n    n::::nj::::j i::::i  n::::n    n::::n	 */
/*    M::::::M    M:::::M    M::::::Mo::::o     o::::o  E:::::E               n::::n    n::::nj::::j i::::i  n::::n    n::::n	 */
/*    M::::::M     MMMMM     M::::::Mo::::o     o::::o  E:::::E       EEEEEE  n::::n    n::::nj::::j i::::i  n::::n    n::::n	 */
/*    M::::::M               M::::::Mo:::::ooooo:::::oEE::::::EEEEEEEE:::::E  n::::n    n::::nj::::ji::::::i n::::n    n::::n	 */
/*    M::::::M               M::::::Mo:::::::::::::::oE::::::::::::::::::::E  n::::n    n::::nj::::ji::::::i n::::n    n::::n	 */
/*    M::::::M               M::::::M oo:::::::::::oo E::::::::::::::::::::E  n::::n    n::::nj::::ji::::::i n::::n    n::::n	 */
/*    MMMMMMMM               MMMMMMMM   ooooooooooo   EEEEEEEEEEEEEEEEEEEEEE  nnnnnn    nnnnnnj::::jiiiiiiii nnnnnn    nnnnnn	 */
/*                                                                                            j::::j                         	 */
/*                                                                                  jjjj      j::::j                         	 */
/*                                                                                 j::::jj   j:::::j                         	 */
/*                                                                                 j::::::jjj::::::j                         	 */
/*                                                                                  jj::::::::::::j                          	 */
/*                                                                                    jjj::::::jjj                           	 */
/*                                                                                       jjjjjj                              	 */
/*																																 */
/**********************************************************************************************************************************/   

/*

Configuration Macros
  - Compilers
	- MO_COMPILER_MSVC   (Microsoft)
	- MO_COMPILER_MSVC7  (Microsoft VC++ 7)
	- MO_COMPILER_MSVC9  (Microsoft VC++ 9)
	- MO_COMPILER_GCC    (GCC)

  - Inlining
	- mo_inline
	- mo_inline_always
	- mo_no_inline

  - Platform
	- MO_PLATFORM_DARWIN
	- MO_PLATFORM_MAC
	- MO_PLATFORM_WINDOWS
	- MO_PLATFORM_IS_PTR_64_BIT		(defined when platform uses 64 bit pointers)
	- MO_PLATFORM_IS_32_BIT			(defined when platform is 32 bit)
	- MO_PLATFORM_IS_64_BIT			(defined when platform is 64 bit)
	- MO_PLATFORM_USE_JSVALUE_32	(defined when the JSValue is for 32 bit)
	- MO_PLATFORM_USE_JSVALUE_64	(defined when the JSValue is for 64 bit)

  - API
	- mo_export
	- mo_import
	- mo_hidden
	- mo_api	(equals MO_EXPORT when MO_LIB is defined, otherwise MO_IMPORT)
	- MO_LIB	(define when building as a library to consume)

  - Misc
	- MO_DEFAULT_DISABLE_ASSERTIONS (defined when not in debug mode to disable assertions by default)
	- MO_DISABLE_ASSERTIONS			(define to disable assertions, regardless of build mode)
	- MO_EXCLUDE_COMMON_HEADERS		(define to exclude the Core/Common.h header from being included by default)

*/

/************************************************************************/
/*                              COMPILERS                               */
/************************************************************************/

#if defined(_MSC_VER)
#define MO_COMPILER_MSVC 1
#if _MSC_VER < 1400
#define MO_COMPILER_MSVC7 1
#elif _MSC_VER < 1600
#define MO_COMPILER_MSVC9 1
#endif
#endif

#if defined(__GNUC__)
#define MO_COMPILER_GCC 1
#endif


/************************************************************************/
/*                           ARCHITECTURES                              */
/************************************************************************/
#if defined(__ia64__)
#define MO_CPU_IA64 1		// Itanium, IA-64
#if !defined(__LP64__)
#define MO_CPU_IA64_32 1	// Itanium  32-bit mode
#endif
#endif

// 64-bit x86_64, AMD64, Intel64
#if defined(__x86_64__) || defined(_M_X64)
#define MO_CPU_X86_64 1
#endif

// 32-bit x86, i386
#if defined(__i386__) || defined(i386) || defined(_M_IX86) || defined(_X86_) || defined(__THW_INTEL)
#define MO_CPU_X86 1
#endif

// 32-bit PowerPC
#if defined(__ppc__) || defined(__PPC__) || defined(__powerpc__) || defined(__POWERPC__) || defined(__powerpc) || defined(_M_PPC) || defined(__PPC)
#define MO_CPU_PPC 1
#endif

// 64-bit PowerPC
#if defined(__ppc64__) || defined(__PPC64__)
#define MO_CPU_PPC_64 1
#endif

// ARM
#if defined(arm) || defined(__arm__) || defined(ARM) || defined(_ARM_)
#define MO_CPU_ARM 1
#endif


/************************************************************************/
/*                              BITNESS                                 */
/************************************************************************/
#if defined(__i386__) || defined(i386) || defined(_M_IX86) || defined(_X86_) || defined(__THW_INTEL)
#define MO_PLATFORM_IS_32_BIT 1
#endif

#if defined(__x86_64__) || defined(_WIN64) || defined(__LP64__)
#define MO_PLATFORM_IS_64_BIT 1
#endif

/************************************************************************/
/*                              INLINING                                */
/************************************************************************/
#ifndef mo_inline
#define mo_inline inline
#endif

#ifndef mo_inline_always
#if defined(MO_COMPILER_GCC)
#define mo_inline_always inline __attribute__((__always_inline__))
#elif defined(MO_COMPILER_MSVC)
#define mo_inline_always __forceinline
#else
#define mo_inline_always
#endif
#endif

#ifndef mo_no_inline
#if defined(MO_COMPILER_GCC)
#define mo_no_inline __attribute__((__noinline__))
#elif defined(MO_COMPILER_MSVC)
#define mo_no_inline __declspec(noinline)
#else
#define mo_no_inline
#endif
#endif

/************************************************************************/
/*                              PLATFORMS                               */
/************************************************************************/

// DARWIN (iOS, MacOSX)
#if defined(__APPLE__)
#include <Availability.h>
#include <AvailabilityMacros.h>
#include <TargetConditionals.h>

#define MO_PLATFORM_DARWIN 1
#endif

// MacOSX
#if defined(MO_PLATFORM_DARWIN) && defined(TARGET_OS_MAC)
#define MO_PLATFORM_MAC 1
#endif

// Windows
#if defined(WIN32) || defined(_WIN32)
#define MO_PLATFORM_WINDOWS 1
#endif


/************************************************************************/
/*                              EXPORT/IMPORT                           */
/************************************************************************/
#if defined(MO_PLATFORM_WINDOWS) && !defined(MO_COMPILER_GCC)
#define mo_export __declspec(dllexport)
#define mo_import __declspec(dllimport)
#define mo_hidden
#elif defined(MO_COMPILER_GCC)
#define mo_export __attribute__((visibility("default")))
#define mo_import mo_export
#define mo_hidden __attribute__((visibility("hidden")))
#else
#define mo_export
#define mo_import
#define mo_hidden
#endif

#if defined(MO_LIB)
#define mo_api mo_export
#else
#define mo_api mo_import
#endif

#define mo_extern extern "C"

/************************************************************************/
/*                                 OTHER								*/
/************************************************************************/

#if defined(MO_COMPILER_MSVC)
#pragma warning(disable : 4018) // signed/unsigned compare
#pragma warning(disable : 4251) // needs dll interface
#pragma warning(disable : 4355) // 'this' used in base member initialize list
#pragma warning(disable : 4996) // vc++ 8 deprecation
#pragma warning(disable : 4351) // elements of array '...' will be default initialized
#pragma warning(disable : 4675) // resolved overload was found by argument lookup
#pragma warning(disable : 4099) // vcXX.pdb not found
#endif


#ifdef NDEBUG
#define MO_DEFAULT_DISABLE_ASSERTIONS 1
#else
#define MO_DEFAULT_DISABLE_ASSERTIONS 0
#endif//MO_DEFAULT_DISABLE_ASSERTIONS

#ifndef MO_DISABLE_ASSERTIONS
#define MO_DISABLE_ASSERTIONS MO_DEFAULT_DISABLE_ASSERTIONS
#endif//MO_DISABLE_ASSERTIONS

#if defined(MO_PLATFORM_WINDOWS)
#include "Platform/Win/Platform.h"
#endif

#include "Platform/Libs.h"
#include "Platform/Types.h"
#include "Platform/Macros.h"
#include "Platform/Extras.h"

// by default, include the core common headers
#ifndef MO_EXCLUDE_COMMON_HEADERS
#include "Core/Common.h"
#endif//MO_EXCLUDE_COMMON_HEADERS

#endif