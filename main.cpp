#include "Platform/Configuration.h"
#include "AST/ProgramNode.h"
#include "CodeGen/IRCodeGen.h"
#include "Parser/Parser.h"
#include "Runtime/GlobalContext.h"
#include "Runtime/SourceCode.h"

using namespace MoEnjin;

int _tmain(int argc, _TCHAR* argv[])
{
	if(argc < 3 || argc > 4)
	{
		printf("ERROR: Wrong number of arguments supplied.\n\n");
		printf("USAGE:\n");
		printf("\tMoCompiler.exe [--s] [SOURCE-JAVASCRIPT-FILE] [OUTPUT-IR-FILE]\n");

		return 0;
	}

	mo_bool isStandalone = mo_false;

	if(argc == 4)
		isStandalone = (UTF8String::FromUTF16(argv[1]) == "--s");
	
	UTF8String jsFilePath = UTF8String::FromUTF16(argv[isStandalone ? 2 : 1]);
	UTF8String outputFilePath = UTF8String::FromUTF16(argv[isStandalone ? 3 : 2]);
	GlobalContext* globalContext = new GlobalContext();
	SourceCode sourceCode = SourceCode::FromFile(jsFilePath);
	UTF8StringCollection parameters;
	Parser parser(globalContext, sourceCode, parameters, Parser::ParserStrictnessNormal, Parser::ParserModeProgram);
	ProgramNode* program = parser.Parse<ProgramNode>();
	
	if(program)
	{
		IRCodeGen generator;
		generator.Generate(program);
		generator.Save(outputFilePath);

		printf("Done, OK\n");
	}
	else
	{
		printf("Done, Failed\n");
	}

	// wait for user input to shutdown the console
	if(isStandalone)
	{
		printf("\nPress any key to exit...");
		getchar();
	}

	return 0;
}