#include "AST/CaseClauseNode.h"

namespace MoEnjin
{
	CaseClauseNode::CaseClauseNode(Expression* expression, StatementList* statements)
		: mExpression(expression)
		, mStatements(statements) { }
}