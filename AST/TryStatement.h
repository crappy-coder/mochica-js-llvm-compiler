#ifndef MOENJIN_AST_TRYSTATEMENT_H
#define MOENJIN_AST_TRYSTATEMENT_H

#include "Platform/Configuration.h"
#include "AST/Statement.h"

namespace MoEnjin
{
	class TryStatement : public Statement
	{
		public:
			TryStatement(mo_int32 lineNumber, const UTF8String& exceptionId, Statement* tryBlock, Statement* catchBlock, Statement* finallyBlock);

			virtual const NodeType GetType() const { return NODE_TRY; }

			const UTF8String& GetExceptionId() const { return mExceptionId; }

			mo_bool HasCatchBlock() const { return !!mCatchBlock; }
			mo_bool HasFinallyBlock() const { return !!mFinallyBlock; }

			Statement* GetTryBlock() const { return mTryBlock; }
			Statement* GetCatchBlock() const { return mCatchBlock; }
			Statement* GetFinallyBlock() const { return mFinallyBlock; }

		private:
			UTF8String mExceptionId;
			Statement* mTryBlock;
			Statement* mCatchBlock;
			Statement* mFinallyBlock;
	};
}

#endif