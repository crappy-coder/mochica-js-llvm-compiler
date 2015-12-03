#ifndef MOENJIN_CODEGEN_IRSCOPE_H
#define MOENJIN_CODEGEN_IRSCOPE_H

#include "Platform/Configuration.h"
#include "CodeGen/SymbolTable.h"
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Value.h>

namespace MoEnjin
{
	struct IRScope
	{
		public:
			IRScope(IRValue callframe)
				: mCallFrame(callframe)
				, mNativeCallFrame(mo_null)
				, mBlock(mo_null) { }

			IRScope(const IRScope& other)
				: mBlock(other.mBlock)
				, mCallFrame(other.mCallFrame)
				, mNativeCallFrame(other.mNativeCallFrame) { }

			IRBasicBlock GetBlock() { return mBlock; }
			void SetBlock(IRBasicBlock block) { mBlock = block; }

			IRValue GetCallFrame() { return mCallFrame; }
			void SetCallFrame(IRValue callFrame) { mCallFrame = callFrame; }

			IRValue GetNativeCallFrame() { return mNativeCallFrame; }
			void SetNativeCallFrame(IRValue callFrame) { mNativeCallFrame = callFrame; }

			const mo_bool HasBlock() const { return !!mBlock; }
			const mo_bool HasCallFrame() const { return !!mCallFrame; }
			const mo_bool HasNativeCallFrame() const { return !!mNativeCallFrame; }

			SymbolTable& GetLocals() { return mLocals; }
			SymbolTable& GetParameters() { return mParameters; }

			IRValue AddLocal(const UTF8String& name, const IRValue value) { return mLocals.Set(name, value.Get()); }
			IRValue AddParameter(const UTF8String& name, const IRValue value) { return mParameters.Set(name, value.Get()); }

			const IRValue GetLocal(const UTF8String& name) const { return mLocals.Get(name); }
			IRValue GetLocal(const UTF8String& name) { return mLocals.Get(name); }

			const IRValue GetParameter(const UTF8String& name) const { return mParameters.Get(name); }
			IRValue GetParameter(const UTF8String& name) { return mParameters.Get(name); }

		private:
			IRBasicBlock mBlock;
			IRValue mCallFrame;
			IRValue mNativeCallFrame;
			SymbolTable mLocals;
			SymbolTable mParameters;
	};

	typedef std::vector<IRScope> IRScopeCollection;

	struct IRScopeRef
	{
		public:
			IRScopeRef(IRScopeCollection* stack, mo_unsigned index)
				: mStack(stack), mIndex(index) { }

			const mo_unsigned GetIndex() const { return mIndex; }

			IRScope* operator->() { return &mStack->at(mIndex); }

		private:
			IRScopeCollection* mStack;
			mo_unsigned mIndex;
	};
}

#endif