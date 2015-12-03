#ifndef MOENJIN_AST_THROWABLEEXPRESSION_H
#define MOENJIN_AST_THROWABLEEXPRESSION_H

#include "Platform/Configuration.h"

namespace MoEnjin
{
	class ThrowableExpression
	{
		public:
			ThrowableExpression()
				: mDivot(static_cast<mo_uint32>(-1))
				, mStartPosition(static_cast<mo_uint16>(-1))
				, mEndPosition(static_cast<mo_uint16>(-1)) { }

			ThrowableExpression(mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos)
				: mDivot(divot)
				, mStartPosition(startPos)
				, mEndPosition(endPos) { }

			mo_uint32 GetDivot() const { return mDivot; }
			mo_uint16 GetStartPosition() const { return mStartPosition; }
			mo_uint16 GetEndPosition() const { return mEndPosition; }

			void SetExceptionLocation(mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos)
			{
				mDivot = divot;
				mStartPosition = startPos;
				mEndPosition = endPos;
			}

		private:
			mo_uint32 mDivot;
			mo_uint16 mStartPosition;
			mo_uint16 mEndPosition;
	};
}

#endif