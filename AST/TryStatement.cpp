#include "AST/TryStatement.h"

namespace MoEnjin
{
	TryStatement::TryStatement(mo_int32 lineNumber, const UTF8String& exceptionId, Statement* tryBlock, Statement* catchBlock, Statement* finallyBlock)
		: Statement(lineNumber)
		, mExceptionId(exceptionId)
		, mTryBlock(tryBlock)
		, mCatchBlock(catchBlock)
		, mFinallyBlock(finallyBlock) {}
}