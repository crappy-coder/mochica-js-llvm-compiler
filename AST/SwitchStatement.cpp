#include "AST/SwitchStatement.h"

namespace MoEnjin
{
	SwitchStatement::SwitchStatement(int lineNumber, Expression* expression, CaseBlockNode* block)
		: Statement(lineNumber)
		, mExpression(expression)
		, mBlock(block) { }
}