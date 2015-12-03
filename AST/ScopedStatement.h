#ifndef MOENJIN_AST_SCOPEDSTATEMENT_H
#define MOENJIN_AST_SCOPEDSTATEMENT_H

#include "Platform/Configuration.h"
#include "AST/CodeFeatures.h"
#include "AST/FunctionList.h"
#include "AST/Statement.h"
#include "AST/VariableList.h"
#include "Runtime/SourceCode.h"

namespace MoEnjin
{
	class StatementList;
	class GlobalContext;
	class ScopedStatement : public Statement
	{
		public:
			ScopedStatement(GlobalContext* globalContext, mo_int32 lineNumber, mo_bool inStrictContext);
			ScopedStatement(GlobalContext* globalContext, mo_int32 lineNumber, const SourceCode& source, StatementList* statements, const VariableList& variables, const FunctionList& functions, IdentifierSet& capturedVariables, CodeFeatures features, mo_int32 constantCount);

			virtual const NodeType GetType() const { return NODE_SCOPE; }

			GlobalContext* GetGlobalContext() const { return mGlobalContext; }
			StatementList* GetStatements() const { return mStatements; }

			const SourceCode& GetSource() const { return mSource; }
			CodeFeatures GetFeatures() const { return mFeatures; }

			void SetFeatures(CodeFeatures features) { mFeatures = features; }
			void SetUsesArguments() { mFeatures |= CodeFeaturesArguments; }

			mo_bool UsesEval() const { return (mFeatures & CodeFeaturesEval) != CodeFeaturesNone; }
			mo_bool UsesArguments() const { return ((mFeatures & CodeFeaturesArguments) != CodeFeaturesNone) && ((mFeatures & CodeFeaturesShadowsArguments) == CodeFeaturesNone); }
			mo_bool UsesThis() const { return ((mFeatures & CodeFeaturesThis) != CodeFeaturesNone); }

			mo_bool IsStrictMode() const { return ((mFeatures & CodeFeaturesStrictMode) != CodeFeaturesNone); }

			mo_bool HasCapturedVariables() const { return (mCapturedVariables.size() > 0); }

			mo_bool NeedsActivation() const { return (HasCapturedVariables()) || ((mFeatures & (CodeFeaturesEval | CodeFeaturesWith | CodeFeaturesCatch)) != CodeFeaturesNone); }
			mo_bool NeedsActivationForMoreThanVariables() const { return ((mFeatures & (CodeFeaturesEval | CodeFeaturesWith | CodeFeaturesCatch)) != CodeFeaturesNone); }

			mo_uint32 GetCapturedVariableCount() const { return mCapturedVariables.size(); }
			mo_bool Captures(const UTF8String& id) { return (mCapturedVariables.find(id.Get()) != mCapturedVariables.end()); }

			const VariableList& GetVariables() const { return mVariables; }
			const FunctionList& GetFunctions() const { return mFunctions; }

			mo_int32 GetNeededConstantCount() { return mConstantCount + 2; }

		protected:
			void SetSource(const SourceCode& source) { mSource = source; }

			GlobalContext* mGlobalContext;
			SourceCode mSource;
			StatementList* mStatements;
			CodeFeatures mFeatures;
			FunctionList mFunctions;
			VariableList mVariables;
			IdentifierSet mCapturedVariables;
			mo_int32 mConstantCount;
	};
}

#endif