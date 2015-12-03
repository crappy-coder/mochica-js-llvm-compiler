#ifndef MOENJIN_AST_FUNCTIONNODE_H
#define MOENJIN_AST_FUNCTIONNODE_H

#include "Platform/Configuration.h"
#include "AST/ScopedStatement.h"
#include "AST/ParameterNode.h"

namespace MoEnjin
{
	class FunctionNode : public ScopedStatement
	{
		public:
			FunctionNode(GlobalContext* globalContext, mo_int32 lineNumber, mo_bool inStrictContext);
			FunctionNode(GlobalContext* globalContext, mo_int32 lineNumber, const SourceCode& source, StatementList* statements, const VariableList& variables, const FunctionList& functions, IdentifierSet& capturedVariables, CodeFeatures features, mo_int32 constantCount, mo_bool hasReturnValue = mo_false);

			virtual const NodeType GetType() const { return NODE_FUNCTION; }

			NodeType GetOwnerType() const { return mOwnerType; }
			FunctionNode* GetParent() const { return mParent; }
			const UTF8StringCollection& GetParameters() const { return mParameters; }
			const mo_uint32 GetParameterCount() const { return mParameters.size(); }

			const mo_bool HasReturnValue() const { return mHasReturnValue; }
			const mo_bool HasName() const { return !mName.IsEmpty(); }
			const mo_bool HasInferredName() const { return !mInferredName.IsEmpty(); }

			const UTF8String& GetName() const { return mName; }
			const UTF8String& GetInferredName() const { return mInferredName; }
			const UTF8String& GetNameOrInferredName() const { return (mName.IsEmpty() ? mInferredName : mName); }
			const UTF8String GetQualifiedName() const;

			void SetParent(FunctionNode* value) { mParent = value; }
			void SetInferredName(const UTF8String& name)
			{
				MO_ASSERT(!name.IsNull());
				mInferredName = name;
			}

			void FinishParsing(const SourceCode& source, ParameterNode* first, const UTF8String& name, NodeType ownerType);

		private:
			FunctionNode* mParent;
			NodeType mOwnerType;
			UTF8String mName;
			UTF8String mInferredName;
			UTF8StringCollection mParameters;
			mo_bool mHasReturnValue;
	};
}

#endif