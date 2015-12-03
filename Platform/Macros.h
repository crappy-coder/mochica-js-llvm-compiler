#ifndef MOENJIN_PLATFORM_MACROS_H
#define MOENJIN_PLATFORM_MACROS_H

#define MO_STATIC_CLASS(ClassName)					\
	private:										\
		ClassName() { }								\
		ClassName(const ClassName&);				\
		ClassName& operator=(const ClassName&)

#define MO_NON_COPYABLE_CLASS(ClassName)			\
	private:										\
		ClassName(const ClassName&);				\
		ClassName& operator=(const ClassName&)

#if defined(MO_PLATFORM_WINDOWS)
#define MO_INT64_FORMAT "I64"
#elif defined(MO_PLATFORM_DARWIN)
#define MO_INT64_FORMAT "q"
#else
#define MO_INT64_FORMAT "ll"
#endif

#define MO_VA_NUM_ARGS(...) MO_VA_NUM_ARGS_IMPL_II(MO_VA_NUM_ARGS_IMPL_I(__VA_ARGS__,10,9,8,7,6,5,4,3,2,1))
#define MO_VA_NUM_ARGS_IMPL_I(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,N,...) N
#define MO_VA_NUM_ARGS_IMPL_II(N) N

#define MO_MACRO_DISPATCHER(func, ...) MO_MACRO_DISPATCHER_I(func, MO_VA_NUM_ARGS(__VA_ARGS__))
#define MO_MACRO_DISPATCHER_I(func, nargs) MO_MACRO_DISPATCHER_II(func, nargs)
#define MO_MACRO_DISPATCHER_II(func, nargs) func ## nargs


#define MO_MIN(a, b) (a < b ? a : b)
#define MO_MAX(a, b) (a > b ? a : b)

#define MO_STRINGIFY(s) #s
#define MO_WIDE(s) L ## s

#define MO_CONCAT(...) MO_MACRO_DISPATCHER(MO_CONCAT, __VA_ARGS__)(__VA_ARGS__)
#define MO_CONCAT1(s1) s1
#define MO_CONCAT2(s1, s2) s1 ## s2
#define MO_CONCAT3(s1, s2, s3) s1 ## s2 ## s3
#define MO_CONCAT4(s1, s2, s3, s4) s1 ## s2 ## s3 ## s4
#define MO_CONCAT5(s1, s2, s3, s4, s5) s1 ## s2 ## s3 ## s4 ## s5
#define MO_CONCAT6(s1, s2, s3, s4, s5, s6) s1 ## s2 ## s3 ## s4 ## s5 ## s6


#if defined(MO_USE_CSTYLE_CASTING)
#define MO_CAST(t,obj)				(t)(obj)
#define MO_CAST_PTR(t,obj)			(t*)(obj)
#else
#define MO_CAST(t,obj)				static_cast<t>(obj)
#define MO_CAST_PTR(t,obj)			static_cast<t*>(obj)
#endif

#define MO_OBJECT_OFFSET_OF(class, member) (reinterpret_cast<mo_ptrdiff>(&(reinterpret_cast<class*>(0x4000)->member)) - 0x4000)

#define MO_UNUSED(x)				((void)x)
#define MO_ENTER_DEBUGGER()			(__debugbreak())

#define MO_DELETE(obj)				do {delete obj; obj = 0;} while(0)
#define MO_DELETE_ARRAY(arr)		do {delete arr; arr = 0;} while(0)

#define MO_TEMPLATE_WITH_TYPE(T)			template <typename T> 
#define MO_TEMPLATE_WITH_TYPES(T1,T2)		template <typename T1, typename T2> 
#define MO_TEMPLATE_WITH_CLASS(C)			template <class C> 
#define MO_TEMPLATE_WITH_CLASSES(C1, C2)	template <class C1, class C2> 

#define MO_LOG_MESSAGE(msg, ...)	mo_debug_print(msg "\n", ##__VA_ARGS__)
#define MO_LOG_WARNING(msg, ...)	mo_debug_print("warning: " msg "\n", ##__VA_ARGS__)
#define MO_LOG_ERROR(msg, ...)		mo_debug_print("error: " msg "\n", ##__VA_ARGS__)

// TODO - remove the following macros when js error handling is implemented
//        for now we use these, as they will be the same name used in the real
//        function calls.
#define jsThrowNativeError(ty)					mo_debug_print(#ty " occurred in %s (%d)\n", __FILE__, __LINE__)
#define jsThrowNativeErrorWithMessage(ty, msg)	mo_debug_print(#ty " (" msg ") in %s (%d)\n", __FILE__, __LINE__)

#endif