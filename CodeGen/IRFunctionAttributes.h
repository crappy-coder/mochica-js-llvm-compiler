#ifndef MOENJIN_CODEGEN_IRFUNCTIONATTRIBUTES_H
#define MOENJIN_CODEGEN_IRFUNCTIONATTRIBUTES_H

namespace MoEnjin
{
	enum IRFunctionAttributes
	{
		kIRFunctionAttributeNone = 0,
		kIRFunctionAttributeNoUnwind = 1 << 0,
		kIRFunctionAttributeNoReturn = 1 << 1,
		kIRFunctionAttributeReturnsTwice = 1 << 2
	};
}

#endif