#ifndef MOENJIN_AST_NUMBEREXPRESSION_H
#define MOENJIN_AST_NUMBEREXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"

namespace MoEnjin
{
	class NumberExpression : public Expression
	{
		public:
			NumberExpression(mo_int32 lineNumber, mo_double value);

			virtual const NodeType GetType() const { return NODE_NUMBER; }

			const mo_double GetValue() const { return mValue; }
			void SetValue(mo_double value) { mValue = value; }

		private:
			mo_double mValue;
	};
}

#endif