#ifndef MOENJIN_AST_THROWABLEPREFIXEDSUBEXPRESSION_H
#define MOENJIN_AST_THROWABLEPREFIXEDSUBEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/ThrowableExpression.h"

namespace MoEnjin
{
	class ThrowablePrefixedSubExpression : public ThrowableExpression
	{
		public:
			ThrowablePrefixedSubExpression()
				: mSubDivot(0), mSubStartPosition(0) { }

			ThrowablePrefixedSubExpression(mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos)
				: ThrowableExpression(divot, startPos, endPos)
				, mSubDivot(0), mSubStartPosition(0) { }

			void SetExpressionInfo(mo_uint32 divot, mo_uint16 start)
			{
				MO_ASSERT(divot >= GetDivot());

				if((divot - GetDivot()) & ~0xFFFF)
					return;

				mSubDivot = divot - GetDivot();
				mSubStartPosition = start;
			}

		private:
			mo_uint16 mSubDivot;
			mo_uint16 mSubStartPosition;
	};
}

#endif