#ifndef MOENJIN_CODEGEN_STUBCALL_H
#define MOENJIN_CODEGEN_STUBCALL_H

#include "Platform/Configuration.h"
#include "CodeGen/IREmitter.h"
#include "CodeGen/IRFunctionScope.h"
#include "CodeGen/IRTypeBuilder.h"
#include "CodeGen/IRValue.h"
#include "Runtime/JSStubDefs.h"
#include "Runtime/JSValue.h"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>

namespace MoEnjin
{
	class StubCall
	{
		typedef std::vector<llvm::Value*> ValueCollection;

		public:
			StubCall(IREmitter* emitter, const IRValue& stubFn)
				: mEmitter(emitter)
				, mStubFunction(stubFn) { }

			void Add(mo_bool value, mo_bool encode = mo_true)
			{
				if(encode)
				{
					Add(JSValue::MakeBoolean(value));
					return;
				}

				Add(IRTypeBuilder::GetConstBool(value));
			}

			void Add(const mo_int8 value, mo_bool encode = mo_true)
			{
				if(encode)
				{
					Add(JSValue::MakeNumber(value));
					return;
				}

				Add(IRTypeBuilder::GetConstInt8(value));
			}

			void Add(const mo_int16 value, mo_bool encode = mo_true)
			{
				if(encode)
				{
					Add(JSValue::MakeNumber(value));
					return;
				}

				Add(IRTypeBuilder::GetConstInt16(value));
			}

			void Add(const mo_int32 value, mo_bool encode = mo_true)
			{
				if(encode)
				{
					Add(JSValue::MakeNumber(value));
					return;
				}

				Add(IRTypeBuilder::GetConstInt32(value));
			}

			void Add(const mo_uint8 value, mo_bool encode = mo_true)
			{
				if(encode)
				{
					Add(JSValue::MakeNumber(value));
					return;
				}

				Add(IRTypeBuilder::GetConstUInt8(value));
			}

			void Add(const mo_uint16 value, mo_bool encode = mo_true)
			{
				if(encode)
				{
					Add(JSValue::MakeNumber(value));
					return;
				}

				Add(IRTypeBuilder::GetConstUInt16(value));
			}

			void Add(const mo_uint32 value, mo_bool encode = mo_true)
			{
				if(encode)
				{
					Add(JSValue::MakeNumber(value));
					return;
				}

				Add(IRTypeBuilder::GetConstUInt32(value));
			}

			void Add(const mo_double value, mo_bool encode = mo_true)
			{
				if(encode)
				{
					Add(JSValue::MakeDouble(value));
					return;
				}

				Add(IRTypeBuilder::GetConstDouble(value));
			}

			void Add(const JSValue& value)
			{
				Add(IRTypeBuilder::GetConstInt64(JSValue::Encode(value)));
			}

			void Add(llvm::Value* value)
			{
				mArguments.push_back(value);
			}

			//IRValue Add(llvm::Value* value)
			//{
			//	if(!mStubFunction.IsNull())
			//	{
			//		mArguments.push_back(value);
			//		return IRValue(value);
			//	}

			//	llvm::Type* type = value->getType();
			//	UTF8String name = value->getName();

			//	// convert pointer type
			//	if(type->isPointerTy())
			//		mArguments.push_back(mEmitter->EmitPtrCast(value, IRInt64Ty, name + "_addr"));
			//	else if(type->isIntegerTy(64))
			//		mArguments.push_back(value);
			//	else
			//	{
			//		// convert non-64bit integer type
			//		if(type->isIntegerTy())
			//			mArguments.push_back(mEmitter->EmitIntCast(value, IRInt64Ty));
			//		else
			//		{
			//			// TODO - add other conversion types as they come up...
			//			MO_ASSERT_NOT_REACHED();
			//		}
			//	}

			//	return IRValue(value);
			//}

			IRValue Call()
			{
				IRValue jsStackFrameValue = mEmitter->GetFunctionScope()->HasExecutionContext() ? 
											mEmitter->GetFunctionScope()->GetExecutionContext() : IRTypeBuilder::GetConstNullPtr(IRBytePtrTy);

				mArguments.insert(mArguments.begin(), jsStackFrameValue.Get());

				return mEmitter->EmitCall(mStubFunction, mArguments);
			}

		private:
			IREmitter* mEmitter;
			IRValue mStubFunction;
			ValueCollection mArguments;
	};
}

#endif