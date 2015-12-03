#ifndef MOENJIN_AST_ARRAYEXPRESSION_H
#define MOENJIN_AST_ARRAYEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/Element.h"
#include "AST/Expression.h"

namespace MoEnjin
{
	class ArrayExpression : public Expression
	{
		public:
			ArrayExpression(mo_int32 lineNumber, mo_int32 elision);
			ArrayExpression(mo_int32 lineNumber, mo_int32 elision, Element* element);
			ArrayExpression(mo_int32 lineNumber, Element* element);

			virtual const NodeType GetType() const { return NODE_ARRAY; }

			Element* GetElement() const { return mElement; }
			mo_int32 GetElision() const { return mElision; }
			mo_bool IsOptional() const { return mOptional; }

		private:
			Element* mElement;
			mo_int32 mElision;
			mo_bool mOptional;
	};
}

#endif