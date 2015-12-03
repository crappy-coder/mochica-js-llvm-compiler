#include "AST/IfElseStatement.h"

namespace MoEnjin
{
	IfElseStatement::IfElseStatement(mo_int32 lineNumber, Expression* condition, Statement* ifBlock, Statement* elseBlock)
		: IfStatement(lineNumber, condition, ifBlock)
		, mElseBlock(elseBlock) {}
}