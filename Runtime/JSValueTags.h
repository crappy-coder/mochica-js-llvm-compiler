#ifndef MOENJIN_RUNTIME_JSVALUETAGS_H
#define MOENJIN_RUNTIME_JSVALUETAGS_H

#include "Platform/Configuration.h"

namespace MoEnjin
{
	enum JSValueNullTag			{ JSValueNull };
	enum JSValueUndefinedTag	{ JSValueUndefined };
	enum JSValueTrueTag			{ JSValueTrue };
	enum JSValueFalseTag		{ JSValueFalse };
	enum JSValueDoubleTag		{ JSValueDouble };

#if defined(MO_PLATFORM_USE_JSVALUE_32)
	enum { JSValueDataIntTag		= 0xffffffff };
	enum { JSValueDataBooleanTag	= 0xfffffffe };
	enum { JSValueDataNullTag		= 0xfffffffd };
	enum { JSValueDataUndefinedTag	= 0xfffffffc };
	enum { JSValueDataObjectTag		= 0xfffffffb };
	enum { JSValueDataStringTag		= 0xfffffffa };
	enum { JSValueDataEmptyTag		= 0xfffffff9 };
	enum { JSValueDataLowestTag		= JSValueDataEmptyTag };
#elif defined(MO_PLATFORM_USE_JSVALUE_64)

#define DoubleEncodeOffset 0x1000000000000ll
#define TagTypeNumber 0xffff000000000000ll

#define TagBitTypeOther 0x2ll
#define TagBitBool      0x4ll
#define TagBitUndefined 0x8ll

#define ValueFalse     (TagBitTypeOther | TagBitBool | false)
#define ValueTrue      (TagBitTypeOther | TagBitBool | true)
#define ValueUndefined (TagBitTypeOther | TagBitUndefined)
#define ValueNull      (TagBitTypeOther)

#define TagMask (TagTypeNumber | TagBitTypeOther)

#define ValueEmpty   0x0ll
#define ValueDeleted 0x4ll
#endif

}

#endif