#ifndef MOENJIN_CODEGEN_IRVALUE_H
#define MOENJIN_CODEGEN_IRVALUE_H

#include "Platform/Configuration.h"
#include "Platform/LLVMDecls.h"

#include <llvm/IR/Argument.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalValue.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Value.h>

// TODO: add other forward decls..

namespace llvm {
	class SwitchInst;
	class CallInst;
}

namespace MoEnjin
{
	template<class T> class IRValueWrapper;
	
	template<typename T>
	class IRValueWrapper
	{
		friend class IRValueWrapper<llvm::Value*>;
		friend class IRValueWrapper<llvm::Argument*>;
		friend class IRValueWrapper<llvm::BasicBlock*>;
		friend class IRValueWrapper<llvm::Constant*>;
		friend class IRValueWrapper<llvm::Function*>;
		friend class IRValueWrapper<llvm::GlobalValue*>;
		friend class IRValueWrapper<llvm::GlobalVariable*>;
		friend class IRValueWrapper<llvm::Instruction*>;

		public:
			typedef llvm::Value base_type;
			typedef typename RemovePtr<T>::T value_type;
			typedef value_type* pointer_type;

		public:
			static IRValueWrapper<pointer_type> Null;

		public:
			IRValueWrapper()
				: mValue(0) { }

			IRValueWrapper(pointer_type value)
				: mValue(value) { }

			IRValueWrapper(const IRValueWrapper& other)
				: mValue(other.mValue) { }

			template<typename U>
			IRValueWrapper(const IRValueWrapper<U>& other)
				: mValue(other.mValue) { }

			~IRValueWrapper() { mValue = mo_null; }

			mo_bool IsNull() const { return (mValue == mo_null); }
			mo_bool IsArgument() const { return !IsNull() && llvm::isa<llvm::Argument>(mValue); }
			mo_bool IsConstant() const { return !IsNull() && llvm::isa<llvm::Constant>(mValue); }
			mo_bool IsFunction() const { return !IsNull() && llvm::isa<llvm::Function>(mValue); }
			mo_bool IsBasicBlock() const { return !IsNull() && llvm::isa<llvm::BasicBlock>(mValue); }
			mo_bool IsGlobalValue() const { return !IsNull() && llvm::isa<llvm::GlobalValue>(mValue); }
			mo_bool IsGlobalVariable() const { return !IsNull() && llvm::isa<llvm::GlobalVariable>(mValue); }
			mo_bool IsInstruction() const { return !IsNull() && llvm::isa<llvm::Instruction>(mValue); }

			pointer_type Get() const { return llvm::dyn_cast_or_null<value_type>(mValue); }
			pointer_type Get() { return llvm::dyn_cast_or_null<value_type>(mValue); }

			template<typename U>
			U* GetAs() const { return llvm::dyn_cast_or_null<U>(mValue); }

			void Reset() { mValue = mo_null; }
			void Set(pointer_type value) { mValue = llvm::cast_or_null<base_type>(value); }

			void Delete()
			{
				pointer_type p = llvm::cast_or_null<value_type>(mValue);

				if(p)
					delete p;

				mValue = mo_null;
			}

			pointer_type operator *() const { return llvm::cast_or_null<value_type>(mValue); }
			pointer_type operator->() const { return llvm::cast_or_null<value_type>(mValue); }
			mo_bool operator!() const { return IsNull(); }
			
			operator pointer_type () { return llvm::cast_or_null<value_type>(mValue); }
			operator pointer_type () const { return llvm::cast_or_null<value_type>(mValue); }

			IRValueWrapper& operator=(const IRValueWrapper& other);
			IRValueWrapper& operator=(pointer_type other);

			template<typename U> IRValueWrapper& operator=(const IRValueWrapper<U>& other);
			template<typename U> IRValueWrapper& operator=(U* other);

			template<typename T, typename U>
			friend mo_bool operator==(const IRValueWrapper& a, const IRValueWrapper& b);

			template<typename T, typename U>
			friend mo_bool operator==(const IRValueWrapper& a, U* b);

			template<typename T, typename U>
			friend mo_bool operator==(T* a, const IRValueWrapper& b);

			template<typename T, typename U>
			friend mo_bool operator!=(const IRValueWrapper& a, const IRValueWrapper& b);

			template<typename T, typename U>
			friend mo_bool operator!=(const IRValueWrapper& a, U* b);

			template<typename T, typename U>
			friend mo_bool operator!=(T* a, const IRValueWrapper& b);

		protected:
			base_type* mValue;
	};

	typedef IRValueWrapper<llvm::Value*> IRValue;
	typedef IRValueWrapper<llvm::Argument*> IRArgument;
	typedef IRValueWrapper<llvm::BasicBlock*> IRBasicBlock;
	typedef IRValueWrapper<llvm::Constant*> IRConstant;
	typedef IRValueWrapper<llvm::Function*> IRFunction;
	typedef IRValueWrapper<llvm::GlobalValue*> IRGlobalValue;
	typedef IRValueWrapper<llvm::GlobalVariable*> IRGlobalVariable;
	typedef IRValueWrapper<llvm::Instruction*> IRInstruction;
	typedef IRValueWrapper<llvm::SwitchInst*> IRSwitchInstruction;
	typedef IRValueWrapper<llvm::CallInst*> IRCallInst;

	template<typename T>
	IRValueWrapper<T> IRValueWrapper<T>::Null(mo_null);

	mo_inline llvm::raw_ostream &operator<<(llvm::raw_ostream& ostr, const IRValue& value) {
		value->print(ostr);
		return ostr;
	}

	mo_inline llvm::raw_ostream &operator<<(llvm::raw_ostream& ostr, const IRArgument& value) {
		value->print(ostr);
		return ostr;
	}

	mo_inline llvm::raw_ostream &operator<<(llvm::raw_ostream& ostr, const IRConstant& value) {
		value->print(ostr);
		return ostr;
	}

	mo_inline llvm::raw_ostream &operator<<(llvm::raw_ostream& ostr, const IRFunction& value) {
		value->print(ostr);
		return ostr;
	}

	mo_inline llvm::raw_ostream &operator<<(llvm::raw_ostream& ostr, const IRBasicBlock& value) {
		value->print(ostr);
		return ostr;
	}

	mo_inline llvm::raw_ostream &operator<<(llvm::raw_ostream& ostr, const IRGlobalValue& value) {
		value->print(ostr);
		return ostr;
	}

	mo_inline llvm::raw_ostream &operator<<(llvm::raw_ostream& ostr, const IRGlobalVariable& value) {
		value->print(ostr);
		return ostr;
	}

	template<typename T> 
	mo_inline IRValueWrapper<T>& IRValueWrapper<T>::operator=(const IRValueWrapper<T>& other)
	{
		mValue = other.mValue;
		return *this;
	}

	template<typename T>
	mo_inline IRValueWrapper<T>& IRValueWrapper<T>::operator=(pointer_type other)
	{
		mValue = other;
		return *this;
	}

	template<typename T> template<typename U>
	mo_inline IRValueWrapper<T>& IRValueWrapper<T>::operator=(const IRValueWrapper<U>& other)
	{
		mValue = other.mValue;
		return *this;
	}
	
	template<typename T> template<typename U>
	mo_inline IRValueWrapper<T>& IRValueWrapper<T>::operator=(U* other)
	{
		mValue = other;
		return *this;
	}

	template<typename T, typename U>
	mo_inline mo_bool operator==(const IRValueWrapper<T>& a, const IRValueWrapper<U>& b)
	{
		return (a.Get() == b.Get());
	}

	template<typename T, typename U>
	mo_inline mo_bool operator==(const IRValueWrapper<T>& a, U* b)
	{
		return (a.Get() == b);
	}

	template<typename T, typename U>
	mo_inline mo_bool operator==(T* a, const IRValueWrapper<U>& b)
	{
		return (a == b.Get());
	}

	template<typename T, typename U>
	mo_inline mo_bool operator!=(const IRValueWrapper<T>& a, const IRValueWrapper<U>& b)
	{
		return (a.Get() != b.Get());
	}

	template<typename T, typename U>
	mo_inline mo_bool operator!=(const IRValueWrapper<T>& a, U* b)
	{
		return (a.Get() != b);
	}

	template<typename T, typename U>
	mo_inline mo_bool operator!=(T* a, const IRValueWrapper<U>& b)
	{
		return (a != b.Get());
	}
}

#endif