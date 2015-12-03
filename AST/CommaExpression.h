#ifndef MOENJIN_AST_COMMAEXPRESSION_H
#define MOENJIN_AST_COMMAEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"

namespace MoEnjin
{
	class CommaExpression : public Expression
	{
		typedef std::vector<Expression*> ExpressionCollection;

		public:
			CommaExpression(mo_int32 lineNumber, Expression* expr1, Expression* expr2);

			virtual const NodeType GetType() const { return NODE_COMMA; }

			mo_uint32 GetCount() const { return mExpressions.size(); }
			Expression* GetAt(mo_uint32 index) const { return mExpressions[index]; }

			void Append(Expression* expr) { mExpressions.push_back(expr); }

		private:
			ExpressionCollection mExpressions;
	};
}

#endif