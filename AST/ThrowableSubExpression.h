#ifndef MOENJIN_AST_THROWABLESUBEXPRESSION_H
#define MOENJIN_AST_THROWABLESUBEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/ThrowableExpression.h"

namespace MoEnjin
{
	class ThrowableSubExpression : public ThrowableExpression
	{
		public:
			ThrowableSubExpression()
				: mSubDivot(0), mSubEndPosition(0) { }

			ThrowableSubExpression(mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos)
				: ThrowableExpression(divot, startPos, endPos)
				, mSubDivot(0), mSubEndPosition(0) { }

			void SetExpressionInfo(mo_uint32 divot, mo_uint16 end)
			{
				MO_ASSERT(divot <= GetDivot());

				if((GetDivot() - divot) & ~0xFFFF)
					return;

				mSubDivot = GetDivot() - divot;
				mSubEndPosition = end;
			}

		private:
			mo_uint16 mSubDivot;
			mo_uint16 mSubEndPosition;
	};
}

#endif