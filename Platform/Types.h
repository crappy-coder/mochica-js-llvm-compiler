#ifndef MOENJIN_PLATFORM_TYPES_H
#define MOENJIN_PLATFORM_TYPES_H

/************************************************************************/
/*                                TYPES                                 */
/************************************************************************/

// platform uses 64 bit pointers
#if defined(_WIN64) || defined(__LP64__)
#define MO_PLATFORM_IS_PTR_64_BIT 1
#endif

typedef double			mo_double;

typedef unsigned char	mo_byte;
typedef unsigned		mo_unsigned;

typedef char			mo_utf8char;
typedef wchar_t			mo_utf16char;

typedef unsigned int	mo_atom;

// Microsoft C++ Compiler
#if defined(_MSC_VER)
typedef signed char			mo_char;
typedef char				mo_int8;
typedef short				mo_int16;
typedef int					mo_int32;
typedef __int64				mo_int64;
typedef long				mo_long;
typedef unsigned char		mo_uchar;
typedef unsigned char		mo_uint8;
typedef unsigned short		mo_uint16;
typedef unsigned int		mo_uint32;
typedef unsigned long		mo_ulong;
typedef unsigned __int64	mo_uint64;

#if defined(_WIN64)
typedef int64				mo_intptr;
typedef uint64				mo_uintptr;
typedef __int64				mo_ptrdiff;
#else
typedef long				mo_intptr;
typedef unsigned long		mo_uintptr;
typedef __w64 int			mo_ptrdiff;
#endif

// GNU Compiler
#elif defined(__GNUC__)
typedef signed char			mo_char;
typedef char				mo_int8;
typedef short				mo_int16;
typedef int					mo_int32;
typedef long				mo_long;
typedef unsigned char		mo_uchar;
typedef unsigned char		mo_uint8;
typedef unsigned short		mo_uint16;
typedef unsigned int		mo_uint32;
typedef unsigned long		mo_ulong;
typedef long				mo_intptr;
typedef unsigned long		mo_uintptr;

#if defined(__LP64__)
typedef unsigned long		mo_uint64;
typedef long				mo_int64;
#else
typedef unsigned long long	mo_uint64;
typedef long long			mo_int64;
#endif

#else
#error UNSUPPORTED COMPILER!
#endif

typedef std::string			mo_utf8string;
typedef std::wstring		mo_utf16string;
typedef mo_utf8string		mo_string;

#define mo_utf8stringptr	mo_utf8char*
#define mo_utf16stringptr	mo_utf16char*
#define mo_stringptr		mo_utf8stringptr

#if defined(MO_PLATFORM_WINDOWS)
typedef bool				mo_bool;

#define mo_true				true
#define mo_false			false
#else
typedef int					mo_bool;

#define mo_true				1
#define mo_false			0
#endif


#if defined(__cplusplus)
#define mo_null				0
#else
#define mo_null				((void *)0)
#endif


#if !defined(MO_PLATFORM_USE_JSVALUE_32) && !defined(MO_PLATFORM_USE_JSVALUE_64)
#if defined(MO_PLATFORM_IS_64_BIT) && defined(MO_PLATFORM_WINDOWS)
#define MO_PLATFORM_USE_JSVALUE_64 1
#else
#define MO_PLATFORM_USE_JSVALUE_32 1
#endif
#endif

namespace MoEnjin {
	const size_t mo_not_found = static_cast<size_t>(-1);
	const mo_atom mo_bad_atom = static_cast<mo_atom>(-1);

	template <typename Ty> struct RemovePtr				{ typedef Ty T; };
	template <typename Ty> struct RemovePtr<Ty*>		{ typedef Ty T; };

	template <typename Ty> struct RemoveRef				{ typedef Ty T; };
	template <typename Ty> struct RemoveRef<Ty&>		{ typedef Ty T; };

	template <typename Ty> struct RemoveConst			{ typedef Ty T; };
	template <typename Ty> struct RemoveConst<const Ty>	{ typedef Ty T; };
}

using MoEnjin::mo_not_found;
using MoEnjin::mo_bad_atom;

#endif