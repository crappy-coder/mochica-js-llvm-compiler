#include "AST/IfStatement.h"

namespace MoEnjin
{
	IfStatement::IfStatement(mo_int32 lineNumber, Expression* condition, Statement* block)
		: Statement(lineNumber)
		, mCondition(condition)
		, mBlock(block) { }
}