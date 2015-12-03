#ifndef MOENJIN_CODEGEN_IRFUNCTIONTYPE_H
#define MOENJIN_CODEGEN_IRFUNCTIONTYPE_H

namespace MoEnjin
{
	enum IRFunctionType 
	{
		kIRFunctionTypeStub,
		kIRFunctionTypeBuiltin,
		kIRFunctionTypeJS,
		kIRFunctionTypeMain,
		kIRFunctionTypeProgram
	};
}

#endif