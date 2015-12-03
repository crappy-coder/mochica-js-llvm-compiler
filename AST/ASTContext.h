#ifndef MOENJIN_AST_ASTCONTEXT_H
#define MOENJIN_AST_ASTCONTEXT_H

#include "Platform/Configuration.h"
#include "AST/ASTDecls.h"
#include "AST/CodeFeatures.h"
#include "AST/Operators.h"
#include "AST/FunctionList.h"
#include "AST/PropertyType.h"
#include "AST/ThrowableExpression.h"
#include "AST/VariableList.h"

namespace MoEnjin
{
	enum VarAttribute
	{
		VarAttributeNone = 0,
		VarAttributeIsConstant = 1,
		VarAttributeHasInitializer = 2
	};
	
	class GlobalContext;
	class Lexer;
	class SourceCode;

	class ASTContext
	{
		public:
			struct BinaryOpInfo
			{
				BinaryOpInfo() { }

				BinaryOpInfo(mo_int32 s, mo_int32 d, mo_int32 e, mo_bool rha)
					: start(s), divot(d), end(e), hasAssignment(rha) { }

				BinaryOpInfo(const BinaryOpInfo& lhs, const BinaryOpInfo& rhs)
					: start(lhs.start), divot(rhs.start), end(rhs.end)
					, hasAssignment(lhs.hasAssignment || rhs.hasAssignment) { }

				mo_int32 start;
				mo_int32 divot;
				mo_int32 end;
				mo_bool hasAssignment;
			};

			struct BinaryOperand
			{
				Expression* node;
				BinaryOpInfo info;
			};

			struct AssignmentInfo
			{
				AssignmentInfo() { }
				AssignmentInfo(Expression* n, mo_int32 s, mo_int32 d, mo_int32 i, Operator o)
					: node(n), start(s), divot(d), initAssignments(i), op(o) { }

				Expression* node;
				mo_int32 start;
				mo_int32 divot;
				mo_int32 initAssignments;
				Operator op;
			};

		private:
			struct Scope
			{
				friend class ASTContext;

				public:
					Scope()
						: mFeatures(0)
						, mConstantCount(0) { }

				private:
					VariableList mVariableDecls;
					FunctionList mFunctions;
					int mFeatures;
					int mConstantCount;
			};

		public:
			ASTContext(GlobalContext* globalContext, SourceCode* source, const Lexer* lexer);

			mo_int32 GetFeatures() const { return mScope.mFeatures; }
			mo_int32 GetConstantCount() const { return mScope.mConstantCount; }
			mo_int32 GetEvalCount() const { return mEvalCount; }

			VariableList& GetVariableDeclarations() { return mScope.mVariableDecls; }
			FunctionList& GetFunctions() { return mScope.mFunctions; }

			Expression* CreateLogicalNot(Expression* expression);
			Expression* CreateUnaryPlus(Expression* expression);
			Expression* CreateVoid(Expression* expression);
			Expression* CreateThis();
			Expression* CreateResolve(const UTF8String& id, mo_int32 start);
			Expression* CreateObjectLiteral();
			Expression* CreateObjectLiteral(PropertyListNode* properties);
			Expression* CreateArray(Element* elements);
			Expression* CreateArray(Element* elements, mo_int32 elisions);
			Expression* CreateArray(mo_int32 elisions);
			Expression* CreateNumber(mo_double value, mo_bool isConstant = mo_true);
			Expression* CreateString(const UTF8String& value);
			Expression* CreateBoolean(mo_bool value);
			Expression* CreateNull();
			Expression* CreateBracketAccess(Expression* base, Expression* property, mo_bool propertyHasAssignments, mo_int32 start, mo_int32 divot, mo_int32 end);
			Expression* CreateDotAccess(Expression* base, const UTF8String& property, mo_int32 start, mo_int32 divot, mo_int32 end);
			Expression* CreateRegExp(const UTF8String& pattern, const UTF8String& flags, mo_int32 start);
			Expression* CreateNew(Expression* expression, ArgumentsNode* arguments, mo_int32 start, mo_int32 divot, mo_int32 end);
			Expression* CreateNew(Expression* expression, mo_int32 start, mo_int32 end);
			Expression* CreateConditional(Expression* condition, Expression* lhs, Expression* rhs);
			Expression* CreateAssignResolve(const UTF8String& id, Expression* rhs, mo_bool rhsHasAssignment, mo_int32 start, mo_int32 divot, mo_int32 end);
			Expression* CreateFunction(const UTF8String& name, FunctionNode* function, ParameterNode* parameters, mo_int32 openBracePosition, mo_int32 closeBracePosition, mo_int32 bodyBeginLine, mo_int32 bodyEndLine);
			Expression* CreateFunctionCall(Expression* function, ArgumentsNode* arguments, mo_int32 start, mo_int32 divot, mo_int32 end);
			Expression* CreateAssignment(mo_int32& assignmentStackDepth, Expression* rhs, mo_int32 initialCount, mo_int32 currentCount, mo_int32 lastTokenEnd);
			Expression* CreateAssignment(Operator op, Expression* left, Expression* right, mo_bool leftHasAssignments, mo_bool rightHasAssignments, mo_int32 start, mo_int32 divot, mo_int32 end);
			Expression* CreatePrefix(Expression* expression, Operator op, mo_int32 start, mo_int32 divot, mo_int32 end);
			Expression* CreatePostfix(Expression* expression, Operator op, mo_int32 start, mo_int32 divot, mo_int32 end);
			Expression* CreateTypeOf(Expression* expression);
			Expression* CreateDelete(Expression* expression, mo_int32 start, mo_int32 divot, mo_int32 end);
			Expression* CreateNegate(Expression* expression);
			Expression* CreateBitwiseNot(Expression* expression);

			StatementList* CreateStatementList();
			Statement* CreateFunctionDeclaration(const UTF8String& name, FunctionNode* function, ParameterNode* parameters, mo_int32 openBracePosition, mo_int32 closeBracePosition, mo_int32 bodyBeginLine, mo_int32 bodyEndLine);
			Statement* CreateBlock(StatementList* statements, mo_int32 begin, mo_int32 end);
			Statement* CreateExpression(Expression* expression, mo_int32 begin, mo_int32 end);
			Statement* CreateIf(Expression* condition, Statement* trueBlock, mo_int32 begin, mo_int32 end);
			Statement* CreateIf(Expression* condition, Statement* trueBlock, Statement* falseBlock, mo_int32 begin, mo_int32 end);
			Statement* CreateForLoop(Expression* initializer, Expression* condition, Expression* iter, Statement* statements, mo_bool isFirstExprVarDecl, mo_int32 begin, mo_int32 end);
			Statement* CreateForInLoop(const UTF8String& id, Expression* initializer, Expression* iter, Statement* statements, mo_int32 start, mo_int32 divot, mo_int32 end, mo_int32 initBegin, mo_int32 initEnd, mo_int32 beginLine, mo_int32 endLine);
			Statement* CreateForInLoop(Expression* lhs, Expression* iter, Statement* statements, mo_int32 start, mo_int32 divot, mo_int32 end, mo_int32 beginLine, mo_int32 endLine);
			Statement* CreateEmpty();
			Statement* CreateVar(Expression* expression, mo_int32 begin, mo_int32 end);
			Statement* CreateReturn(Expression* expression, mo_int32 start, mo_int32 end, mo_int32 beginLine, mo_int32 endLine);
			Statement* CreateBreak(mo_int32 start, mo_int32 end, mo_int32 beginLine, mo_int32 endLine);
			Statement* CreateBreak(const UTF8String& id, mo_int32 start, mo_int32 end, mo_int32 beginLine, mo_int32 endLine);
			Statement* CreateContinue(mo_int32 start, mo_int32 end, mo_int32 beginLine, mo_int32 endLine);
			Statement* CreateContinue(const UTF8String& id, mo_int32 start, mo_int32 end, mo_int32 beginLine, mo_int32 endLine);
			Statement* CreateTry(const UTF8String& id, Statement* tryBlock, Statement* catchBlock, Statement* finallyBlock, mo_int32 begin, mo_int32 end);
			Statement* CreateSwitch(Expression* expression, ClauseListNode* firstClauses, CaseClauseNode* defaultClause, ClauseListNode* secondClauses, mo_int32 begin, mo_int32 end);
			Statement* CreateWhile(Expression* expression, Statement* statement, mo_int32 begin, mo_int32 end);
			Statement* CreateDoWhile(Expression* expression, Statement* statement, mo_int32 begin, mo_int32 end);
			Statement* CreateLabel(const UTF8String& id, Statement* statement, mo_int32 begin, mo_int32 end);
			Statement* CreateWith(Expression* expression, Statement* statement, mo_int32 start, mo_int32 end, mo_int32 beginLine, mo_int32 endLine);
			Statement* CreateThrow(Expression* expression, mo_int32 start, mo_int32 end, mo_int32 beginLine, mo_int32 endLine);
			Statement* CreateDebugger(mo_int32 beginLine, mo_int32 endLine);
			Statement* CreateConst(ConstDeclExpression* decls, mo_int32 beginLine, mo_int32 endLine);

			ConstDeclExpression* CreateConstDecl(const UTF8String& name, Expression* initializer, ConstDeclExpression* next = mo_null);
			CommaExpression* CreateComma(Expression* lhs, Expression* rhs);
			FunctionNode* CreateFunction(mo_bool inStrictContext);
			FunctionNode* CreateFunction(StatementList* statements, const VariableList& variables, const FunctionList& functions, IdentifierSet& capturedVariables, CodeFeatures features, mo_int32 constantCount, mo_int32 openBracePosition, mo_int32 closeBracePosition, mo_int32 bodyBeginLine, mo_bool hasReturn = mo_false);
			PropertyNode* CreateGetterOrSetter(PropertyType type, const UTF8String& name, ParameterNode* parameters, FunctionNode* function, mo_int32 openBracePosition, mo_int32 closeBracePosition, mo_int32 bodyBeginLine, mo_int32 bodyEndLine);
			PropertyNode* CreateGetterOrSetter(PropertyType type, mo_double name, ParameterNode* parameters, FunctionNode* function, mo_int32 openBracePosition, mo_int32 closeBracePosition, mo_int32 bodyBeginLine, mo_int32 bodyEndLine);
			PropertyNode* CreateProperty(const UTF8String& propertyName, Expression* expression, PropertyType type);
			PropertyNode* CreateProperty(mo_double propertyName, Expression* expression, PropertyType type);
			PropertyListNode* CreatePropertyList(PropertyNode* property);
			PropertyListNode* CreatePropertyList(PropertyNode* property, PropertyListNode* tail);
			ArgumentsNode* CreateArguments();
			ArgumentsNode* CreateArguments(ArgumentListNode* arguments);
			ArgumentListNode* CreateArgumentsList(Expression* argument);
			ArgumentListNode* CreateArgumentsList(ArgumentListNode* arguments, Expression* argument);
			Element* CreateElementList(Expression* expression, mo_int32 elisions);
			Element* CreateElementList(Expression* expression, mo_int32 elisions, Element* elements);
			ParameterNode* CreateParameterList(const UTF8String& id);
			ParameterNode* CreateParameterList(const UTF8String& id, ParameterNode* list);
			CaseClauseNode* CreateClause(Expression* expression, StatementList* statements);
			ClauseListNode* CreateClauseList(CaseClauseNode* clause);
			ClauseListNode* CreateClauseList(CaseClauseNode* clause, ClauseListNode* tail);

			void AddVariable(const UTF8String& id, mo_unsigned attributes);
			Expression* CombineCommaExpressions(Expression* list, Expression* init);

			mo_bool PrecedenceIsLowerThanOperatorStack(mo_int32 precedence);
			void FinishBinaryOperation(mo_int32& operandStackDepth, mo_int32& operatorStackDepth);
			void AppendBinaryOperator(mo_int32& operatorStackDepth, mo_int32 op, mo_int32 precedence);			
			Expression* PopBinaryOperand();

			void AppendBinaryExpressionInfo(mo_int32& operandStackDepth, Expression* current, mo_int32 start, mo_int32 lhs, mo_int32 rhs, mo_bool hasAssignments);
			
			void AppendUnaryToken(mo_int32& tokenStackDepth, mo_int32 type, mo_int32 start);
			void RemoveLastUnaryToken(mo_int32& tokenStackDepth);
			mo_int32 GetLastUnaryTokenType();
			mo_int32 GetLastUnaryTokenStart();

			void AppendAssignment(mo_int32& depth, mo_int32 count, Expression* expression, Operator op, mo_int32 start, mo_int32 divot);

		private:
			Expression* CreateBinary(mo_int32 token, BinaryOperand lhs, BinaryOperand rhs);
			Expression* CreateMultiply(Expression* left, Expression* right, mo_bool rightHasAssignments);
			Expression* CreateDivide(Expression* left, Expression* right, mo_bool rightHasAssignments);
			Expression* CreateModulus(Expression* left, Expression* right, mo_bool rightHasAssignments);
			Expression* CreateAdd(Expression* left, Expression* right, mo_bool rightHasAssignments);
			Expression* CreateSubtract(Expression* left, Expression* right, mo_bool rightHasAssignments);
			Expression* CreateLessThan(Expression* left, Expression* right, mo_bool rightHasAssignments);
			Expression* CreateLessThanOrEqual(Expression* left, Expression* right, mo_bool rightHasAssignments);
			Expression* CreateGreaterThan(Expression* left, Expression* right, mo_bool rightHasAssignments);
			Expression* CreateGreaterThanOrEqual(Expression* left, Expression* right, mo_bool rightHasAssignments);
			Expression* CreateBitwiseAnd(Expression* left, Expression* right, mo_bool rightHasAssignments);
			Expression* CreateBitwiseOr(Expression* left, Expression* right, mo_bool rightHasAssignments);
			Expression* CreateBitwiseXOr(Expression* left, Expression* right, mo_bool rightHasAssignments);
			Expression* CreateLeftShift(Expression* left, Expression* right, mo_bool rightHasAssignments);
			Expression* CreateRightShift(Expression* left, Expression* right, mo_bool rightHasAssignments);
			Expression* CreateRightShiftUnsigned(Expression* left, Expression* right, mo_bool rightHasAssignments);

			void IncrementConstantCount() { mScope.mConstantCount++; }
			void SetUsesThis() { mScope.mFeatures |= CodeFeaturesThis; }
			void SetUsesCatch() { mScope.mFeatures |= CodeFeaturesCatch; }
			void SetUsesArguments() { mScope.mFeatures |= CodeFeaturesArguments; }
			void SetUsesWith() { mScope.mFeatures |= CodeFeaturesWith; }
			void SetUsesEval() 
			{ 
				mEvalCount++;
				mScope.mFeatures |= CodeFeaturesEval; 
			}

			const mo_int32 GetLineNumber() const;

			static BinaryOperand MakeOperand(Expression* node, BinaryOpInfo info)
			{
				BinaryOperand op;
				op.node = node;
				op.info = info;

				return op;
			}

			static void SetExceptionLocation(ThrowableExpression* expression, mo_unsigned start, mo_unsigned divot, mo_unsigned end)
			{
				expression->SetExceptionLocation(divot, divot - start, end - divot);
			}

		private:
			GlobalContext* mGlobalContext;
			SourceCode* mSource;
			const Lexer* mLexer;
			Scope mScope;
			mo_int32 mEvalCount;
			std::vector<AssignmentInfo> mAssignmentStack;
			std::vector<BinaryOperand> mBinaryOperandStack;
			std::vector<std::pair<int, int> > mBinaryOperatorStack;
			std::vector<std::pair<int, int> > mUnaryTokenStack;
	};
}

#endif