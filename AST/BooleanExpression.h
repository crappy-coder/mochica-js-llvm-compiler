#ifndef MOENJIN_AST_BOOLEANEXPRESSION_H
#define MOENJIN_AST_BOOLEANEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"

namespace MoEnjin
{
	class BooleanExpression : public Expression
	{
		public:
			BooleanExpression(mo_int32 lineNumber, mo_bool value);

			virtual const NodeType GetType() const { return NODE_BOOLEAN; }

			const mo_bool GetValue() const { return mValue; }
			void SetValue(mo_bool value) { mValue = value; }

		private:
			mo_bool mValue;
	};
}

#endif