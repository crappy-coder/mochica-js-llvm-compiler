#include "Runtime/JSCallFrame.h"
#include "Runtime/JSExecutionEnvironment.h"
#include "Runtime/JSGlobalObject.h"

namespace MoEnjin
{
	JSGlobalObject* JSCallFrame::GetGlobalObject() const
	{
		JSExecutionEnvironment* scope = GetVariableScope();

		MO_ASSERT(scope != mo_null);

		return scope->GetGlobalObject();
	}

	JSCallFrame* JSCallFrame::Create(mo_uint32 argumentCount, mo_uint32 variableCount, JSObject* callee, JSValue thisValue, JSExecutionEnvironment* varEnv, JSExecutionEnvironment* lexEnv)
	{
		return 0;
		//mo_uint32 slots = argumentCount + variableCount + HeaderSize;
		//JSStackBuffer* buffer = &GetGlobalObject()->mStack;
		//JSCallFrame* newFrame = mo_null;

		//if(buffer->Grow(slots))
		//{
		//	newFrame = buffer->GetPosition() - variableCount;
		//	newFrame->SetCallerFrame(this);
		//	newFrame->SetCallee(callee);
		//	newFrame->SetThisValue(thisValue);
		//	newFrame->SetLexicalScope(lexEnv);
		//	newFrame->SetVariableScope(varEnv);
		//	newFrame->SetArgumentCount(argumentCount);

		//	// initialize all arguments to empty
		//	for(mo_int32 i = 0; i < argumentCount; ++i)
		//		newFrame->SetArgument(i, jsEmpty());

		//	// initialize all locals to empty
		//	for(mo_int32 i = 0; i < variableCount; ++i)
		//		newFrame[i] = jsEmpty();
		//}

		//return newFrame;
	}
}