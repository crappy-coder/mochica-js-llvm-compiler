#ifndef MOENJIN_CODEGEN_LLVMUTILS_H
#define MOENJIN_CODEGEN_LLVMUTILS_H

#include "Platform/Configuration.h"
#include <llvm/Support/raw_ostream.h>

namespace MoEnjin
{
	#define LLVM_OUTS llvm::outs()
	#define LLVM_ERRS llvm::errs()

	#define LLVM_PRINT(...) MO_MACRO_DISPATCHER(LLVM_PRINT, __VA_ARGS__)(__VA_ARGS__)
	#define LLVM_PRINT1(expr1) do { llvm::outs() << ##expr1; } while(0)
	#define LLVM_PRINT2(expr1, expr2) do { llvm::outs() << ##expr1 << ##expr2; } while(0)
	#define LLVM_PRINT3(expr1, expr2, expr3) do { llvm::outs() << ##expr1 << ##expr2 << ##expr3; } while(0)
	#define LLVM_PRINT4(expr1, expr2, expr3, expr4) do { llvm::outs() << ##expr1 << ##expr2 << ##expr3 << ##expr4; } while(0)
	#define LLVM_PRINT5(expr1, expr2, expr3, expr4, expr5) do { llvm::outs() << ##expr1 << ##expr2 << ##expr3 << ##expr4 << ##expr5; } while(0)
	#define LLVM_PRINT6(expr1, expr2, expr3, expr4, expr5, expr6) do { llvm::outs() << ##expr1 << ##expr2 << ##expr3 << ##expr4 << ##expr5 << expr6; } while(0)

	#define LLVM_PRINT_LINE(...) MO_MACRO_DISPATCHER(LLVM_PRINT_LINE, __VA_ARGS__)(__VA_ARGS__)
	#define LLVM_PRINT_LINE1(expr1) LLVM_PRINT(expr1, "\n")
	#define LLVM_PRINT_LINE2(expr1, expr2) LLVM_PRINT(expr1, expr2, "\n")
	#define LLVM_PRINT_LINE3(expr1, expr2, expr3) LLVM_PRINT(expr1, expr2, expr3, "\n")
	#define LLVM_PRINT_LINE4(expr1, expr2, expr3, expr4) LLVM_PRINT(expr1, expr2, expr3, expr4, "\n")
	#define LLVM_PRINT_LINE5(expr1, expr2, expr3, expr4, expr5) LLVM_PRINT(expr1, expr2, expr3, expr4, expr5, "\n")

	#define LLVM_FORMAT(...) MO_MACRO_DISPATCHER(LLVM_FORMAT, __VA_ARGS__)(__VA_ARGS__)
	#define LLVM_FORMAT1(fmt) llvm::format(##fmt)
	#define LLVM_FORMAT2(fmt, arg1) llvm::format(##fmt, ##arg1)
	#define LLVM_FORMAT3(fmt, arg1, arg2) llvm::format(##fmt, ##arg1, ##arg2)
	#define LLVM_FORMAT4(fmt, arg1, arg2, arg3) llvm::format(##fmt, ##arg1, ##arg2, ##arg3)
	#define LLVM_FORMAT5(fmt, arg1, arg2, arg3, arg4) llvm::format(##fmt, ##arg1, ##arg2, ##arg3, ##arg4)
	#define LLVM_FORMAT6(fmt, arg1, arg2, arg3, arg4, arg5) llvm::format(##fmt, ##arg1, ##arg2, ##arg3, ##arg4, ##arg5)

	#define LLVM_PRINT_ERR(expr) llvm::errs() << ##expr << "\n"
}

#endif