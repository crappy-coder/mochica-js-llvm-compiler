#ifndef MOENJIN_CODEGEN_IRSTUBEMITTER_H
#define MOENJIN_CODEGEN_IRSTUBEMITTER_H

#include "Platform/Configuration.h"
#include "AST/Operators.h"
#include "CodeGen/IRValue.h"
#include "CodeGen/IRTypes.h"
#include "Runtime/BitOps.h"

namespace MoEnjin
{
	class IRStubEmitter
	{
		friend class IREmitter;
		friend class StubCall;

		public:
			IRStubEmitter(IREmitter* emitter);

			void EmitDebugBreak(mo_int32 startLine, mo_int32 endLine);

			IRValue EmitEnterCall(const IRNativeFunctionBinding* functionBindings, mo_uint32 functionBindingCount, const IRNativeStringCollection& parameterNames, const IRNativeStringCollection& variableNames, mo_bool isStrictCode, mo_bool isFunctionCode);

			void EmitEndCall();

			IRValue EmitNewArray(const IRValue& elements, mo_uint32 length);
			IRValue EmitNewFunc(const UTF8String& name, mo_int32 parameterCount, mo_bool isStrictMode, const IRFunction& nativeFunction);

			IRValue EmitNewPropertyIterator(const IRValue& value);
			IRValue EmitIncrementPropertyIterator(const IRValue& iteratorValue, const IRValue& baseValue, const UTF8String& propertyName);
			IRValue EmitIncrementPropertyIterator(const IRValue& iteratorValue, const IRValue& baseValue, const IRValue& property);

			IRValue EmitThis();
			
			IRValue EmitResolve(const UTF8String& name);
			IRValue EmitResolve(const IRValue& name);

			IRValue EmitResolveBase(const UTF8String& name);
			IRValue EmitResolveBase(const IRValue& name);

			IRValue EmitConstruct(const IRValue& ctor);
			IRValue EmitConstruct(const IRValue& ctor, const IRValue& arguments, mo_uint32 argumentCount);

			IRValue EmitCall(const IRValue& function, const IRValue& baseValue, const IRValue& arguments, mo_uint32 argumentCount);
			IRValue EmitCallByName(const UTF8String& functionName, const IRValue& baseValue, const IRValue& arguments, mo_uint32 argumentCount);
			IRValue EmitCallByName(const IRValue& functionName, const IRValue& baseValue, const IRValue& arguments, mo_uint32 argumentCount);

			IRValue EmitToNumber(const IRValue& value);
			IRValue EmitToBoolean(const IRValue& value);
			IRValue EmitTypeOf(const IRValue& value);

			IRValue EmitNegate(const IRValue& value);

			IRValue EmitPrefixOp(Operator op, const IRValue& baseValue, const UTF8String& propertyName);
			IRValue EmitPrefixOp(Operator op, const IRValue& baseValue, const IRValue& property);

			IRValue EmitPrefixIncrement(const IRValue& baseValue, const IRValue& property);
			IRValue EmitPrefixDecrement(const IRValue& baseValue, const IRValue& property);

			IRValue EmitPostfixOp(Operator op, const IRValue& baseValue, const UTF8String& propertyName);
			IRValue EmitPostfixOp(Operator op, const IRValue& baseValue, const IRValue& property);

			IRValue EmitPostfixIncrement(const IRValue& baseValue, const IRValue& property);
			IRValue EmitPostfixDecrement(const IRValue& baseValue, const IRValue& property);

			IRValue EmitBitOp(const IRValue& leftValue, const IRValue& rightValue, BitOp op);
			IRValue EmitNot(const IRValue& value);

			IRValue EmitLogicalAnd(const IRValue& xValue, const IRValue& yValue);
			IRValue EmitLogicalOr(const IRValue& xValue, const IRValue& yValue);

			IRValue EmitPutVar(const IRValue& base, const UTF8String& propertyName, const IRValue& value);
			IRValue EmitPutVar(const IRValue& base, const IRValue& property, const IRValue& value);
			IRValue EmitPutVarByName(const UTF8String& propertyName, const IRValue& value);
			IRValue EmitPutVarInThis(const UTF8String& propertyName, const IRValue& value);
			IRValue EmitPutVarInThis(const IRValue& property, const IRValue& value);

			IRValue EmitGetVar(const IRValue& base, const UTF8String& propertyName);
			IRValue EmitGetVar(const IRValue& base, const IRValue& property);

			IRValue EmitDeleteVar(const IRValue& base, const UTF8String& propertyName);
			IRValue EmitDeleteVar(const IRValue& base, const IRValue& property);

			IRValue EmitDeleteVarInThis(const UTF8String& propertyName);
			IRValue EmitDeleteVarInThis(const IRValue& property);

			#define DECLARE_EMIT_BINARY_OP(name) IRValue Emit##name(const IRValue& xValue, const IRValue& yValue)

			DECLARE_EMIT_BINARY_OP(Add);
			DECLARE_EMIT_BINARY_OP(Subtract);
			DECLARE_EMIT_BINARY_OP(Multiply);
			DECLARE_EMIT_BINARY_OP(Divide);
			DECLARE_EMIT_BINARY_OP(Modulus);
			DECLARE_EMIT_BINARY_OP(LeftShift);
			DECLARE_EMIT_BINARY_OP(RightShift);
			DECLARE_EMIT_BINARY_OP(UnsignedRightShift);
			DECLARE_EMIT_BINARY_OP(IsLess);
			DECLARE_EMIT_BINARY_OP(IsLessOrEqual);
			DECLARE_EMIT_BINARY_OP(IsGreater);
			DECLARE_EMIT_BINARY_OP(IsGreaterOrEqual);
			DECLARE_EMIT_BINARY_OP(IsInstanceOf);
			DECLARE_EMIT_BINARY_OP(IsIn);
			DECLARE_EMIT_BINARY_OP(Equal);
			DECLARE_EMIT_BINARY_OP(EqualStrict);
			DECLARE_EMIT_BINARY_OP(NotEqual);
			DECLARE_EMIT_BINARY_OP(NotEqualStrict);

			#undef DECLARE_EMIT_BINARY_OP

		private:
			IREmitter* mEmitter;
	};
}

#endif