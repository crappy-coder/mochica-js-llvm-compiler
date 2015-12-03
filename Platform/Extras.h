#ifndef MOENJIN_PLATFORM_EXTRAS_H
#define MOENJIN_PLATFORM_EXTRAS_H

#ifdef __cplusplus
extern "C" {
#endif
	extern void MoReportAssertionFailure(const char* file, int line, const char* function, const char* assertion);
	extern void MoReportError(const char* file, int line, const char* function, const char* msg);
#ifdef __cplusplus
}
#endif

#ifndef MO_CRASH
#define MO_CRASH() do { \
	*(int*)(uintptr_t)0xbbadbeef = 0; \
	((void(*)())0)(); \
} while(0)
#endif//MO_CRASH

#ifdef _DEBUG
#define MO_REPORT_ERROR(err) do { \
	MoReportError(__FILE__, __LINE__, __FUNCTION__, err); \
	} while(0)
#else
#define MO_REPORT_ERROR(err)	((void)0)
#endif

#if MO_DISABLE_ASSERTIONS
#define MO_ASSERT(cond)			((void)0)
#define MO_ASSERT_NOT_REACHED()	((void)0)
#else
#define MO_ASSERT(cond) do \
	if(!(cond)) { \
		MoReportAssertionFailure(__FILE__, __LINE__, __FUNCTION__, #cond); \
		MO_CRASH(); \
	} \
	while(0)

#define MO_ASSERT_NOT_REACHED() do { \
		MoReportAssertionFailure(__FILE__, __LINE__, __FUNCTION__, 0); \
		MO_CRASH(); \
	} while(0)

#endif

namespace MoEnjin
{

	#define mo_debug_print_line(format, ...)     \
		do {                                     \
			mo_debug_print(format, __VA_ARGS__); \
			mo_debug_print("\n");                \
		} while(0)

	mo_inline void mo_debug_print(const mo_utf8stringptr format, ...)
	{
		#if defined(MO_PLATFORM_WINDOWS) && defined(_DEBUG)

		if(!format)
			return;

		va_list mark;
		va_start(mark, format);

		mo_utf8char buf[1024];
		_vsprintf_p(buf, 1024, format, mark);

		OutputDebugStringA(buf);

		va_end(mark);

		#endif
	}

	template<typename TO, typename FROM>
	mo_inline TO bitwise_cast(FROM from)
	{
		MO_ASSERT(sizeof(TO) == sizeof(FROM));

		union {
			FROM f;
			TO t;
		} u;
		
		u.f = from;
		return u.t;
	}

	template<typename TO, typename FROM>
	mo_inline TO mo_cast_type(FROM from)
	{
#if defined(MO_USE_CSTYLE_CASTING)
		return (TO)(from);
#else
		return reinterpret_cast<TO>(from);
#endif
	}

	template <typename TO, typename FROM>
	mo_inline TO mo_cast(FROM from)
	{
#if defined(MO_USE_CSTYLE_CASTING)
		return (TO)(from);
#else
		return static_cast<TO>(from);
#endif
	}

	template <typename TO, typename FROM>
	mo_inline TO mo_cast_const(FROM from)
	{
		return const_cast<TO>(from);
	}

#if defined(MO_PLATFORM_WINDOWS)

	mo_inline mo_bool signbit(mo_double d)
	{
		int c = _fpclass(d);
		return (c >= _FPCLASS_NN && c <= _FPCLASS_NZ);
	}

#endif


template<typename T, mo_uint32 Sz> char (&ArrayLengthFunc(T(&)[Sz]))[Sz];
#if defined(MO_COMPILER_GCC)
template<typename T> char (&ArrayLengthFunc(T(&)[0]))[0];
#endif

#define MO_ARRAY_LENGTH(arr) sizeof(::MoEnjin::ArrayLengthFunc(arr))
}
#endif
