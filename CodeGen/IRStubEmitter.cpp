#include "CodeGen/IRStubEmitter.h"
#include "CodeGen/IRCodeGen.h"
#include "CodeGen/IRConsts.h"
#include "CodeGen/IREmitter.h"
#include "CodeGen/IRType.h"
#include "CodeGen/IRTypeBuilder.h"
#include "CodeGen/StubCall.h"
#include "Platform/LLVM.h"
#include "Runtime/JSStubs.h"
#include "Runtime/JSValue.h"

namespace MoEnjin
{
	#define DEFINE_STUB_CALL(fn) StubCall stubCall(mEmitter, mEmitter->GetTypes().##fn())

	IRStubEmitter::IRStubEmitter(IREmitter* emitter)
		: mEmitter(emitter) { }

	void IRStubEmitter::EmitDebugBreak(mo_int32 startLine, mo_int32 endLine)
	{
		DEFINE_STUB_CALL(GetDebugBreakFn);
		stubCall.Add(startLine, mo_false);
		stubCall.Add(endLine, mo_false);
		stubCall.Call();
	}

	IRValue IRStubEmitter::EmitEnterCall(const IRNativeFunctionBinding* functionBindings, mo_uint32 functionBindingCount, const IRNativeStringCollection& parameterNames, const IRNativeStringCollection& variableNames, mo_bool isStrictCode, mo_bool isFunctionCode)
	{		
		IRValue initData = mEmitter->EmitAllocStruct(mEmitter->mTypes.EnterFunctionCallInitDataTy, IR_INTERNAL_VAR_NAME("init_data"));
		IRValue strings;

		mEmitter->EmitStore(
			mEmitter->EmitStructGEP(initData, 0, 1, IR_INTERNAL_VAR_NAME("init_data_is_function_code")), 
			IRTypeBuilder::GetConstBool(isFunctionCode));

		mEmitter->EmitStore(
			mEmitter->EmitStructGEP(initData, 1, 1, IR_INTERNAL_VAR_NAME("init_data_is_strict_mode")), 
			IRTypeBuilder::GetConstBool(isStrictCode));

		mEmitter->EmitStore(
			mEmitter->EmitStructGEP(initData, 2, 1, IR_INTERNAL_VAR_NAME("init_data_num_parameters")), 
			IRTypeBuilder::GetConstInt32(parameterNames.length));

		mEmitter->EmitStore(
			mEmitter->EmitStructGEP(initData, 3, 1, IR_INTERNAL_VAR_NAME("init_data_num_variables")), 
			IRTypeBuilder::GetConstInt32(variableNames.length));

		mEmitter->EmitStore(
			mEmitter->EmitStructGEP(initData, 4, 1, IR_INTERNAL_VAR_NAME("init_data_num_functions")), 
			IRTypeBuilder::GetConstInt32(functionBindingCount));

		if(parameterNames.length)
		{
			strings = mEmitter->EmitStringArray(parameterNames.values, IR_INTERNAL_VAR_NAME("init_data_parameters_array"));

			mEmitter->EmitStore(
				mEmitter->EmitStructGEP(initData, 5, 1, IR_INTERNAL_VAR_NAME("init_data_parameters")),
				mEmitter->EmitPtrCast(strings, IRTypeBuilder::GetPointerTo(IRBytePtrTy)));
		}

		if(variableNames.length)
		{
			strings = mEmitter->EmitStringArray(variableNames.values, IR_INTERNAL_VAR_NAME("init_data_variables_array"));

			mEmitter->EmitStore(
				mEmitter->EmitStructGEP(initData, 6, 1, IR_INTERNAL_VAR_NAME("init_data_variables")),
				mEmitter->EmitPtrCast(strings, IRTypeBuilder::GetPointerTo(IRBytePtrTy)));
		}

		if(functionBindingCount)
		{
			std::vector<llvm::Value*> functionBindingValues;
			llvm::StructType* nativeBindingType = mEmitter->mTypes.NativeFunctionBindingTy;

			for(mo_int32 i = 0; i < functionBindingCount; ++i)
			{
				const IRNativeFunctionBinding functionBinding = functionBindings[i];
				
				// alloc a native binding object
				llvm::Value* binding = mEmitter->EmitAlloc(nativeBindingType, IR_INTERNAL_VAR_NAME("init_data_function"));

				// get a pointer to each of the members fields
				llvm::Value* bindingNameMember = mEmitter->EmitStructGEP(binding, 0, 1, IR_INTERNAL_VAR_NAME("init_data_function_name"));
				llvm::Value* bindingFunctionMember = mEmitter->EmitStructGEP(binding, 1, 1, IR_INTERNAL_VAR_NAME("init_data_function_function"));
				llvm::Value* bindingParameterCountMember = mEmitter->EmitStructGEP(binding, 2, 1, IR_INTERNAL_VAR_NAME("init_data_function_num_parameters"));
				llvm::Value* bindingStrictMember = mEmitter->EmitStructGEP(binding, 3, 1, IR_INTERNAL_VAR_NAME("init_data_function_is_strict"));

				// store the functions binding value in each member field
				mEmitter->EmitStore(bindingNameMember, IRTypeBuilder::GetStringPtr(mEmitter->GetString(functionBinding.name)));
				mEmitter->EmitStore(bindingParameterCountMember, IRTypeBuilder::GetConstInt32(functionBinding.length));
				mEmitter->EmitStore(bindingFunctionMember, mEmitter->EmitPtrCast(functionBinding.function.Get(), IRBytePtrTy));
				mEmitter->EmitStore(bindingStrictMember, IRTypeBuilder::GetConstBool(functionBinding.inStrictMode));

				// save to list so we can emit the array
				functionBindingValues.push_back(binding);
			}

			// now emit the array of function bindings, this will be sent to the stub function
			IRValue functionBindingArray = mEmitter->EmitArray(nativeBindingType->getPointerTo(), functionBindingValues, IR_INTERNAL_VAR_NAME("init_data_function_array"));

			mEmitter->EmitStore(
				mEmitter->EmitStructGEP(initData, 7, 1, IR_INTERNAL_VAR_NAME("init_data_functions")),
				mEmitter->EmitPtrCast(functionBindingArray, nativeBindingType->getPointerTo()->getPointerTo()));
		}

		DEFINE_STUB_CALL(GetRuntimeStubEnterCallFn);
		stubCall.Add(initData);

		return stubCall.Call();
	}

	void IRStubEmitter::EmitEndCall()
	{
		DEFINE_STUB_CALL(GetRuntimeStubEndCallFn);

		stubCall.Call();
	}

	IRValue IRStubEmitter::EmitNewArray(const IRValue& elements, mo_uint32 length)
	{
		DEFINE_STUB_CALL(GetRuntimeStubNewArrayFn);
		stubCall.Add(elements);
		stubCall.Add(length, mo_false);

		return stubCall.Call();
	}

	IRValue IRStubEmitter::EmitNewFunc(const UTF8String& name, mo_int32 parameterCount, mo_bool isStrictMode, const IRFunction& nativeFunction)
	{
		DEFINE_STUB_CALL(GetRuntimeStubNewFuncFn);
		stubCall.Add(IRTypeBuilder::GetStringPtr(mEmitter->GetString(name)));
		stubCall.Add(IRTypeBuilder::GetConstUInt32(parameterCount));
		stubCall.Add(IRTypeBuilder::GetConstBool(isStrictMode));
		stubCall.Add(mEmitter->EmitBitCast(nativeFunction, IRBytePtrTy, IR_INTERNAL_VAR_NAME("new_func_function_ptr")));

		return stubCall.Call();
	}

	IRValue IRStubEmitter::EmitNewPropertyIterator(const IRValue& value)
	{
		DEFINE_STUB_CALL(GetRuntimeStubNewPropIterFn);
		stubCall.Add(value);

		return stubCall.Call();
	}

	IRValue IRStubEmitter::EmitIncrementPropertyIterator(const IRValue& iteratorValue, const IRValue& baseValue, const UTF8String& propertyName)
	{
		return EmitIncrementPropertyIterator(iteratorValue, baseValue, mEmitter->GetEncodedString(propertyName));
	}

	IRValue IRStubEmitter::EmitIncrementPropertyIterator(const IRValue& iteratorValue, const IRValue& baseValue, const IRValue& property)
	{
		DEFINE_STUB_CALL(GetRuntimeStubIncPropIterFn);
		stubCall.Add(iteratorValue);
		stubCall.Add(baseValue);
		stubCall.Add(property);

		return stubCall.Call();
	}

	IRValue IRStubEmitter::EmitThis()
	{
		DEFINE_STUB_CALL(GetRuntimeStubThisFn);

		return stubCall.Call();
	}

	IRValue IRStubEmitter::EmitResolve(const UTF8String& name)
	{
		return EmitResolve(mEmitter->GetEncodedString(name));
	}

	IRValue IRStubEmitter::EmitResolve(const IRValue& name)
	{
		DEFINE_STUB_CALL(GetRuntimeStubResolveFn);
		stubCall.Add(name);

		return stubCall.Call();
	}

	IRValue IRStubEmitter::EmitResolveBase(const UTF8String& name)
	{
		return EmitResolveBase(mEmitter->GetEncodedString(name));
	}

	IRValue IRStubEmitter::EmitResolveBase(const IRValue& name)
	{
		DEFINE_STUB_CALL(GetRuntimeStubResolveBaseFn);
		stubCall.Add(name);

		return stubCall.Call();
	}

	IRValue IRStubEmitter::EmitConstruct(const IRValue& ctor)
	{
		return EmitConstruct(ctor, IRValue::Null, 0);
	}

	IRValue IRStubEmitter::EmitConstruct(const IRValue& ctor, const IRValue& arguments, mo_uint32 argumentCount)
	{
		DEFINE_STUB_CALL(GetRuntimeStubConstructFn);
		stubCall.Add(ctor);
		stubCall.Add(IRTypeBuilder::GetConstUInt32(argumentCount));
		stubCall.Add(argumentCount == 0 ? IRTypeBuilder::GetConstNullPtr(IRInt64PtrTy) : arguments);

		return stubCall.Call();
	}

	IRValue IRStubEmitter::EmitCall(const IRValue& function, const IRValue& baseValue, const IRValue& arguments, mo_uint32 argumentCount)
	{
		DEFINE_STUB_CALL(GetRuntimeStubCallFuncFn);
		stubCall.Add(function);
		stubCall.Add(baseValue);
		stubCall.Add(IRTypeBuilder::GetConstUInt32(argumentCount));
		stubCall.Add(argumentCount == 0 ? IRTypeBuilder::GetConstNullPtr(IRInt64PtrTy) : arguments);

		return stubCall.Call();
	}

	IRValue IRStubEmitter::EmitCallByName(const UTF8String& functionName, const IRValue& baseValue, const IRValue& arguments, mo_uint32 argumentCount)
	{
		return EmitCallByName(mEmitter->GetEncodedString(functionName), baseValue, arguments, argumentCount);
	}

	IRValue IRStubEmitter::EmitCallByName(const IRValue& functionName, const IRValue& baseValue, const IRValue& arguments, mo_uint32 argumentCount)
	{
		DEFINE_STUB_CALL(GetRuntimeStubCallFuncByNameFn);
		stubCall.Add(functionName);
		stubCall.Add(baseValue);
		stubCall.Add(argumentCount, mo_false);
		stubCall.Add(argumentCount == 0 ? IRTypeBuilder::GetConstNullPtr(IRJSValuePtrTy) : mEmitter->EmitPtrCast(arguments, IRJSValuePtrTy));

		return stubCall.Call();
	}

	IRValue IRStubEmitter::EmitToNumber(const IRValue& value)
	{
		DEFINE_STUB_CALL(GetRuntimeStubToNumberFn);
		stubCall.Add(value);

		return stubCall.Call();
	}

	IRValue IRStubEmitter::EmitToBoolean(const IRValue& value)
	{
		DEFINE_STUB_CALL(GetRuntimeStubToBooleanFn);
		stubCall.Add(value);

		return stubCall.Call();
	}

	IRValue IRStubEmitter::EmitTypeOf(const IRValue& value)
	{
		DEFINE_STUB_CALL(GetRuntimeStubTypeOfFn);
		stubCall.Add(value);

		return stubCall.Call();
	}

	IRValue IRStubEmitter::EmitNegate(const IRValue& value)
	{
		DEFINE_STUB_CALL(GetRuntimeStubNegateFn);
		stubCall.Add(value);

		return stubCall.Call();
	}

	IRValue IRStubEmitter::EmitPrefixOp(Operator op, const IRValue& baseValue, const UTF8String& propertyName)
	{
		return EmitPrefixOp(op, baseValue, mEmitter->GetEncodedString(propertyName));
	}

	IRValue IRStubEmitter::EmitPrefixOp(Operator op, const IRValue& baseValue, const IRValue& property)
	{
		if(op != OpPlusPlus && op != OpMinusMinus)
			return mEmitter->LoadUndefinedValue();

		return (op == OpPlusPlus ? EmitPrefixIncrement(baseValue, property) : EmitPrefixDecrement(baseValue, property));
	}

	IRValue IRStubEmitter::EmitPrefixIncrement(const IRValue& baseValue, const IRValue& property)
	{
		DEFINE_STUB_CALL(GetRuntimeStubPrefixIncFn);
		stubCall.Add(baseValue);
		stubCall.Add(property);

		return stubCall.Call();
	}

	IRValue IRStubEmitter::EmitPrefixDecrement(const IRValue& baseValue, const IRValue& property)
	{
		DEFINE_STUB_CALL(GetRuntimeStubPrefixDecFn);
		stubCall.Add(baseValue);
		stubCall.Add(property);

		return stubCall.Call();
	}

	IRValue IRStubEmitter::EmitPostfixOp(Operator op, const IRValue& baseValue, const UTF8String& propertyName)
	{
		return EmitPostfixOp(op, baseValue, mEmitter->GetEncodedString(propertyName));
	}

	IRValue IRStubEmitter::EmitPostfixOp(Operator op, const IRValue& baseValue, const IRValue& property)
	{
		if(op != OpPlusPlus && op != OpMinusMinus)
			return mEmitter->LoadUndefinedValue();

		return (op == OpPlusPlus ? EmitPostfixIncrement(baseValue, property) : EmitPostfixDecrement(baseValue, property));
	}

	IRValue IRStubEmitter::EmitPostfixIncrement(const IRValue& baseValue, const IRValue& property)
	{
		DEFINE_STUB_CALL(GetRuntimeStubPostfixIncFn);
		stubCall.Add(baseValue);
		stubCall.Add(property);

		return stubCall.Call();
	}

	IRValue IRStubEmitter::EmitPostfixDecrement(const IRValue& baseValue, const IRValue& property)
	{
		DEFINE_STUB_CALL(GetRuntimeStubPostfixDecFn);
		stubCall.Add(baseValue);
		stubCall.Add(property);

		return stubCall.Call();
	}

	IRValue IRStubEmitter::EmitBitOp(const IRValue& leftValue, const IRValue& rightValue, BitOp op)
	{
		DEFINE_STUB_CALL(GetRuntimeStubBitOpFn);
		stubCall.Add(IRTypeBuilder::GetConstUInt32(mo_cast<mo_uint32>(op)));
		stubCall.Add(leftValue);
		stubCall.Add(rightValue);

		return stubCall.Call();
	}

	IRValue IRStubEmitter::EmitNot(const IRValue& value)
	{
		DEFINE_STUB_CALL(GetRuntimeStubNotFn);
		stubCall.Add(value);

		return stubCall.Call();
	}

	IRValue IRStubEmitter::EmitLogicalAnd(const IRValue& xValue, const IRValue& yValue)
	{
		DEFINE_STUB_CALL(GetRuntimeStubLogicalANDFn);
		stubCall.Add(xValue);
		stubCall.Add(yValue);

		return stubCall.Call();
	}

	IRValue IRStubEmitter::EmitLogicalOr(const IRValue& xValue, const IRValue& yValue)
	{
		DEFINE_STUB_CALL(GetRuntimeStubLogicalORFn);
		stubCall.Add(xValue);
		stubCall.Add(yValue);

		return stubCall.Call();
	}

	IRValue IRStubEmitter::EmitPutVar(const IRValue& base, const UTF8String& propertyName, const IRValue& value)
	{
		return EmitPutVar(base, mEmitter->GetEncodedString(propertyName), value);
	}

	IRValue IRStubEmitter::EmitPutVar(const IRValue& base, const IRValue& property, const IRValue& value)
	{
		DEFINE_STUB_CALL(GetRuntimeStubPutVarFn);
		stubCall.Add(base);
		stubCall.Add(property);
		stubCall.Add(value);

		return stubCall.Call();
	}

	IRValue IRStubEmitter::EmitPutVarByName(const UTF8String& propertyName, const IRValue& value)
	{
		DEFINE_STUB_CALL(GetRuntimeStubPutVarByNameFn);
		stubCall.Add(IRTypeBuilder::GetStringPtr(mEmitter->GetString(propertyName)));
		stubCall.Add(value);

		return stubCall.Call();
	}

	IRValue IRStubEmitter::EmitPutVarInThis(const UTF8String& propertyName, const IRValue& value)
	{
		return EmitPutVarInThis(mEmitter->GetEncodedString(propertyName), value);
	}

	IRValue IRStubEmitter::EmitPutVarInThis(const IRValue& property, const IRValue& value)
	{
		DEFINE_STUB_CALL(GetRuntimeStubPutVarInThisFn);
		stubCall.Add(property);
		stubCall.Add(value);

		return stubCall.Call();
	}

	IRValue IRStubEmitter::EmitGetVar(const IRValue& base, const UTF8String& propertyName)
	{
		return EmitGetVar(base, mEmitter->GetEncodedString(propertyName));
	}

	IRValue IRStubEmitter::EmitGetVar(const IRValue& base, const IRValue& property)
	{
		DEFINE_STUB_CALL(GetRuntimeStubGetVarFn);
		stubCall.Add(base);
		stubCall.Add(property);

		return stubCall.Call();
	}

	IRValue IRStubEmitter::EmitDeleteVar(const IRValue& base, const UTF8String& propertyName)
	{
		return EmitDeleteVar(base, mEmitter->GetEncodedString(propertyName));
	}

	IRValue IRStubEmitter::EmitDeleteVar(const IRValue& base, const IRValue& property)
	{
		DEFINE_STUB_CALL(GetRuntimeStubDeleteVarFn);
		stubCall.Add(base);
		stubCall.Add(property);

		return stubCall.Call();
	}

	IRValue IRStubEmitter::EmitDeleteVarInThis(const UTF8String& propertyName)
	{
		return EmitDeleteVarInThis(mEmitter->GetEncodedString(propertyName));
	}

	IRValue IRStubEmitter::EmitDeleteVarInThis(const IRValue& property)
	{
		DEFINE_STUB_CALL(GetRuntimeStubDeleteVarInThisFn);
		stubCall.Add(property);

		return stubCall.Call();
	}


	#define DEFINE_EMIT_BINARY_OP(name) \
		IRValue IRStubEmitter::Emit##name(const IRValue& xValue, const IRValue& yValue) \
		{ \
			DEFINE_STUB_CALL(GetRuntimeStub##name##Fn); \
			stubCall.Add(xValue); \
			stubCall.Add(yValue); \
			return stubCall.Call(); \
		}

	DEFINE_EMIT_BINARY_OP(Add)
	DEFINE_EMIT_BINARY_OP(Subtract)
	DEFINE_EMIT_BINARY_OP(Multiply)
	DEFINE_EMIT_BINARY_OP(Divide)
	DEFINE_EMIT_BINARY_OP(Modulus)
	DEFINE_EMIT_BINARY_OP(LeftShift)
	DEFINE_EMIT_BINARY_OP(RightShift)
	DEFINE_EMIT_BINARY_OP(UnsignedRightShift)
	DEFINE_EMIT_BINARY_OP(IsLess)
	DEFINE_EMIT_BINARY_OP(IsLessOrEqual)
	DEFINE_EMIT_BINARY_OP(IsGreater)
	DEFINE_EMIT_BINARY_OP(IsGreaterOrEqual)
	DEFINE_EMIT_BINARY_OP(IsInstanceOf)
	DEFINE_EMIT_BINARY_OP(IsIn)
	DEFINE_EMIT_BINARY_OP(Equal)
	DEFINE_EMIT_BINARY_OP(EqualStrict)
	DEFINE_EMIT_BINARY_OP(NotEqual)
	DEFINE_EMIT_BINARY_OP(NotEqualStrict)

	#undef DEFINE_EMIT_BINARY_OP
}