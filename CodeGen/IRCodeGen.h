#ifndef MOENJIN_CODEGEN_IRCODEGEN_H
#define MOENJIN_CODEGEN_IRCODEGEN_H

#include "Platform/Configuration.h"
#include "AST/ASTDecls.h"
#include "CodeGen/IRType.h"
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

namespace MoEnjin
{
	class StringTable;
	class IRCodeGen
	{
		friend class IREmitter;

		public:
			static llvm::Constant* JSValueEmptyConstant;
			static llvm::Constant* JSValueUndefinedConstant;
			static llvm::Constant* JSValueNullConstant;
			static llvm::Constant* JSValueTrueConstant;
			static llvm::Constant* JSValueFalseConstant;

		public:
			IRCodeGen();

			llvm::Module* GetModule() const { return mMainModule; }
			llvm::LLVMContext& GetContext();
			llvm::DataLayout* GetDataLayout();
			llvm::StructType* GetStructType(const UTF8String& name) const;

			void Save(const UTF8String& filePath);
			void Generate(ProgramNode* program);

		private:
			void Initialize(const UTF8String& moduleName);

			void DeclareStructType(const UTF8String& name, llvm::StructType* typeModel);
			void GenerateEncodeJSValueMethod();
			
		private:
			llvm::Module* mMainModule;
			llvm::DataLayout* mDataLayout;
			IREmitter* mEmitter;
	};
}

#endif