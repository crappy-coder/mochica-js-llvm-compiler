#ifndef MOENJIN_AST_RESULTKIND_H
#define MOENJIN_AST_RESULTKIND_H

#include "Platform/Configuration.h"

namespace MoEnjin
{
	struct ResultKind
	{
		friend struct OperandResultKind;

		typedef mo_int8 Kind;

		static const Kind Int32Kind		= 0x01;
		static const Kind NumberKind	= 0x04;
		static const Kind StringKind	= 0x08;
		static const Kind NullKind		= 0x10;
		static const Kind BoolKind		= 0x20;
		static const Kind OtherKind		= 0x40;
		static const Kind KindBits		= NumberKind | StringKind | NullKind | BoolKind | OtherKind;

		explicit ResultKind(Kind kind)
			: mKind(kind) { }

		mo_bool IsInt32() const { return (mKind & Int32Kind); }
		mo_bool IsString() const { return (mKind & KindBits) == StringKind; }
		mo_bool IsNumber() const { return (mKind & KindBits) == NumberKind; }
		mo_bool IsNotNumber() const { return !MightBeNumber(); }
		mo_bool MightBeNumber() const { return (mKind & NumberKind) == NumberKind; }

		static ResultKind Null() { return ResultKind(NullKind); }
		static ResultKind Boolean() { return ResultKind(BoolKind); }
		static ResultKind Number() { return ResultKind(NumberKind); }
		static ResultKind String() { return ResultKind(StringKind); }
		static ResultKind Int32() { return ResultKind(Int32Kind | NumberKind); }
		static ResultKind StringOrNumber() { return ResultKind(NumberKind | StringKind); }
		static ResultKind Unknown() { return ResultKind(KindBits); }

		static ResultKind ForBitOp() { return Int32(); }
		static ResultKind ForAdd(ResultKind a, ResultKind b)
		{
			if(a.IsNumber() && b.IsNumber())
				return Number();

			if(a.IsString() || b.IsString())
				return String();

			return StringOrNumber();
		}

		private:
			Kind mKind;
	};

	struct OperandResultKind
	{
		OperandResultKind(ResultKind first = ResultKind::Unknown(), ResultKind second = ResultKind::Unknown())
		{
			mImpl.i = 0;
			mImpl.rds.first = first.mKind;
			mImpl.rds.second = second.mKind;
		}

		ResultKind First() { return ResultKind(mImpl.rds.first); }
		ResultKind Second() { return ResultKind(mImpl.rds.second); }

		int ToInt() { return mImpl.i; }

		static OperandResultKind FromInt(int value)
		{
			OperandResultKind types;
			types.mImpl.i = value;

			return types;
		}

		union {
			struct {
				ResultKind::Kind first;
				ResultKind::Kind second;
			} rds;
			int i;
		} mImpl;
	};
}

#endif