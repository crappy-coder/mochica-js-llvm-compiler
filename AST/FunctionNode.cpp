#include "AST/FunctionNode.h"

namespace MoEnjin
{
	FunctionNode::FunctionNode(GlobalContext* globalContext, mo_int32 lineNumber, mo_bool inStrictContext)
		: ScopedStatement(globalContext, lineNumber, inStrictContext)
		, mParent(mo_null)
		, mHasReturnValue(mo_false)
		, mOwnerType(NODE_UNKNOWN) { }

	FunctionNode::FunctionNode(GlobalContext* globalContext, mo_int32 lineNumber, const SourceCode& source, StatementList* statements, const VariableList& variables, const FunctionList& functions, IdentifierSet& capturedVariables, CodeFeatures features, mo_int32 constantCount, mo_bool hasReturnValue)
		: ScopedStatement(globalContext, lineNumber, source, statements, variables, functions, capturedVariables, features, constantCount)
		, mParent(mo_null)
		, mHasReturnValue(hasReturnValue) 
		, mOwnerType(NODE_UNKNOWN)
	{ 
		mFunctions.SetParent(this);
	}

	const UTF8String FunctionNode::GetQualifiedName() const
	{
		UTF8String name = GetNameOrInferredName();

		if(name.IsEmpty())
			name = "anonymous";

		if(!mParent)
			return name;

		return mParent->GetQualifiedName() + "." + name;
	}

	void FunctionNode::FinishParsing(const SourceCode& source, ParameterNode* first, const UTF8String& name, NodeType ownerType)
	{
		MO_ASSERT(!source.IsNull());

		SetSource(source);
		
		mName = name;
		mOwnerType = ownerType;
		mParameters.clear();

		for(ParameterNode* p = first; p; p = p->GetNext())
			mParameters.push_back(p->GetId());
	}
}