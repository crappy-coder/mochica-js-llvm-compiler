#ifndef MOENJIN_RUNTIME_JSNATIVESTACKFRAME_H
#define MOENJIN_RUNTIME_JSNATIVESTACKFRAME_H

#include "Platform/Configuration.h"
#include "Runtime/JSNativeStubArgument.h"

#define MAX_STUB_ARGS 6

namespace MoEnjin
{
#if defined(MO_CPU_X86_64)

	struct JSNativeStackFrame
	{

	};

#elif defined(MO_CPU_X86)

#if defined(MO_COMPILER_MSVC) || (defined(MO_COMPILER_GCC) && defined(MO_PLATFORM_WINDOWS))
#pragma pack(push)
#pragma pack(4)
#endif

	struct JSNativeStackFrame
	{
		// 0x00 (0)
		void* unused;
		// 0x04 (4)
		JSNativeStubArgument args[MAX_STUB_ARGS];
		// 0x30 (48), maintain 16-byte stack alignment
		void* padding[2];

		// 0x3C (60)
		void* last_ebx;
		void* last_edi;
		void* last_esi;
		void* last_ebp;
		void* last_eip;

		// 0x50 (80)
		void* function;
		// 0x54 (84)
		JSStackFrame* frame;

		// reserved for future use
		void* reserved1;
		void* reserved2;
	};

#if defined(MO_COMPILER_MSVC) || (defined(MO_COMPILER_GCC) && defined(MO_PLATFORM_WINDOWS))
#pragma pack(pop)
#endif

#endif
}

#endif