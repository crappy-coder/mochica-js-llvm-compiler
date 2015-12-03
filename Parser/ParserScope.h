#ifndef MOENJIN_PARSER_PARSERSCOPE_H
#define MOENJIN_PARSER_PARSERSCOPE_H

#include "Platform/Configuration.h"
#include "Runtime/GlobalContext.h"
#include "Runtime/KnownIdentifiers.h"

namespace MoEnjin
{
	typedef mo_unsigned ParserState;
	
    const ParserState ParserStateEmpty					= 0;
    const ParserState ParserStateEval					= 1 << 0;
    const ParserState ParserStateShadowsArguments		= 1 << 1;
    const ParserState ParserStateNeedsFullActivation	= 1 << 2;
    const ParserState ParserStateAllowNewDeclarations	= 1 << 3;
    const ParserState ParserStateStrictMode				= 1 << 4;
    const ParserState ParserStateIsFunction				= 1 << 5;
    const ParserState ParserStateIsFunctionBoundary		= 1 << 6;
    const ParserState ParserStateAll					= ParserStateEval
														| ParserStateShadowsArguments
														| ParserStateNeedsFullActivation
														| ParserStateAllowNewDeclarations
														| ParserStateStrictMode
														| ParserStateIsFunction
														| ParserStateIsFunctionBoundary;
	struct ParserScope
	{
		public:
			struct LabelInfo 
			{
				LabelInfo(const UTF8String& ident, mo_bool loop)
					: id(ident), isLoop(loop) { }

				UTF8String id;
				mo_bool isLoop;
			};

		public:
			ParserScope(const GlobalContext* globalContext, mo_bool isFunction, mo_bool strictMode)
				: mGlobalContext(globalContext)
				, mShadowsArguments(false)
				, mUsesEval(false)
				, mNeedsFullActivation(false)
				, mAllowsNewDeclarations(true)
				, mStrictMode(strictMode)
				, mIsFunction(isFunction)
				, mIsFunctionBoundary(false)
				, mDoesFunctionReturn(false)
				, mHasValidStrictMode(true)
				, mLoopDepth(0)
				, mSwitchDepth(0) { }

			ParserScope(const ParserScope& other)
				: mGlobalContext(other.mGlobalContext)
				, mShadowsArguments(other.mShadowsArguments)
				, mUsesEval(other.mUsesEval)
				, mNeedsFullActivation(other.mNeedsFullActivation)
				, mAllowsNewDeclarations(other.mAllowsNewDeclarations)
				, mStrictMode(other.mStrictMode)
				, mIsFunction(other.mIsFunction)
				, mIsFunctionBoundary(other.mIsFunctionBoundary)
				, mDoesFunctionReturn(other.mDoesFunctionReturn)
				, mHasValidStrictMode(other.mHasValidStrictMode)
				, mLoopDepth(other.mLoopDepth)
				, mSwitchDepth(other.mSwitchDepth) 
			{ 
				if(!other.mLabels.empty())
				{
					LabelCollection::const_iterator end = other.mLabels.end();

					for(LabelCollection::const_iterator it = other.mLabels.begin(); it != end; ++it)
						mLabels.push_back(LabelInfo(it->id, it->isLoop));
				}
			}

			mo_bool AllowsNewDeclarations() const { return mAllowsNewDeclarations; }
			mo_bool IsFunction() const { return mIsFunction; }
			mo_bool IsFunctionBoundary() const { return mIsFunctionBoundary; }
			mo_bool IsInLoop() const { return (mLoopDepth > 0); }
			mo_bool IsStrictModeValid() const { return mHasValidStrictMode; }
			mo_bool CanBreak() const { return (mLoopDepth > 0 || mSwitchDepth > 0); }
			mo_bool CanContinue() const { return (mLoopDepth > 0); }
			mo_bool GetStrictMode() const { return mStrictMode; }
			mo_bool GetShadowsArguments() const { return mShadowsArguments; }
			mo_bool GetDoesFunctionReturn() const { return mDoesFunctionReturn; }

			mo_bool DeclareVariable(const UTF8String& id)
			{
				mo_bool isStrictModeValid = (KnownIdentifiers::eval != id && KnownIdentifiers::arguments != id);

				mHasValidStrictMode = mHasValidStrictMode && isStrictModeValid;
				mDeclaredVariables.insert(id.Get());

				return isStrictModeValid;
			}

			mo_bool DeclareParameter(const UTF8String& id)
			{
				mo_bool isArguments = KnownIdentifiers::arguments == id;
				mo_bool isStrictModeValid = mDeclaredVariables.insert(id).second && KnownIdentifiers::eval != id && !isArguments;

				mHasValidStrictMode = mHasValidStrictMode && isStrictModeValid;

				if(isArguments)
					mShadowsArguments = true;

				return isStrictModeValid;
			}

			void DeclareWrite(const UTF8String& id)
			{
				MO_ASSERT(mStrictMode);
				mWrittenVariables.insert(id);
			}

			void UseVariable(const UTF8String& id, mo_bool isEval)
			{
				mUsesEval |= isEval;
				mUsedVariables.insert(id);
			}

			void PreventNewDeclarations() { mAllowsNewDeclarations = false; }
			void RequireFullActivations() { mNeedsFullActivation = true; }
			void SetStrictMode() { mStrictMode = true; }

			void BeginSwitch() { mSwitchDepth++; }
			void EndSwitch() { mSwitchDepth--; }

			void BeginLoop() { mLoopDepth++; }
			void EndLoop() { MO_ASSERT(mLoopDepth > 0); mLoopDepth--; }

			void SetFunctionDoesReturn() { mDoesFunctionReturn = true; }

			void SetIsFunction()
			{
				mIsFunction = true;
				mIsFunctionBoundary = true;
			}

			void PushLabel(const UTF8String& label, mo_bool isLoop)
			{
				mLabels.push_back(LabelInfo(label.Get(), isLoop));
			}

			void PopLabel()
			{
				MO_ASSERT(mLabels.size() > 0);

				mLabels.pop_back();
			}

			void CollectFreeVariables(ParserScope* nestedScope, mo_bool shouldTrackClosedVariables)
			{
				if(nestedScope->mUsesEval)
					mUsesEval = true;

				IdentifierSet::Iterator end = nestedScope->mUsedVariables.end();

				for(IdentifierSet::Iterator ptr = nestedScope->mUsedVariables.begin(); ptr != end; ++ptr)
				{
					if(nestedScope->mDeclaredVariables.find(*ptr) != nestedScope->mDeclaredVariables.end())
						continue;

					mUsedVariables.insert(*ptr);

					if(shouldTrackClosedVariables)
						mClosedVariables.insert(*ptr);
				}

				if(nestedScope->mWrittenVariables.size() > 0)
				{
					IdentifierSet::Iterator end = nestedScope->mWrittenVariables.end();

					for(IdentifierSet::Iterator ptr = nestedScope->mWrittenVariables.begin(); ptr != end; ++ptr)
					{
						if(nestedScope->mDeclaredVariables.find(*ptr) != nestedScope->mDeclaredVariables.end())
							continue;

						mWrittenVariables.insert(*ptr);
					}
				}
			}

			void GetUncapturedWrittenVariables(IdentifierSet& written)
			{
				IdentifierSet::Iterator end = mWrittenVariables.end();

				for(IdentifierSet::Iterator ptr = mWrittenVariables.begin(); ptr != end; ++ptr)
				{
					if(mDeclaredVariables.find(*ptr) == mDeclaredVariables.end())
						written.insert(*ptr);
				}
			}

			void GetCapturedVariables(IdentifierSet& captured)
			{
				if(mNeedsFullActivation || mUsesEval)
				{
					captured.swap(mDeclaredVariables);
					return;
				}

				for(IdentifierSet::Iterator ptr = mClosedVariables.begin(); ptr != mClosedVariables.end(); ++ptr)
				{
					if(mDeclaredVariables.find(*ptr) == mDeclaredVariables.end())
						continue;

					captured.insert(*ptr);
				}
			}

			LabelInfo* GetLabel(const UTF8String& label)
			{
				if(mLabels.empty())
					return mo_null;

				for(mo_int32 i = mLabels.size(); i > 0; i--)
				{
					if(mLabels.at(i - 1).id == label.Get())
						return &mLabels.at(i - 1);
				}

				return mo_null;
			}

		private:
			typedef std::vector<LabelInfo> LabelCollection;

			const GlobalContext* mGlobalContext;
			LabelCollection mLabels;
			IdentifierSet mDeclaredVariables;
			IdentifierSet mUsedVariables;
			IdentifierSet mClosedVariables;
			IdentifierSet mWrittenVariables;
			mo_bool mShadowsArguments : 1;
			mo_bool mUsesEval : 1;
			mo_bool mNeedsFullActivation : 1;
			mo_bool mAllowsNewDeclarations : 1;
			mo_bool mStrictMode : 1;
			mo_bool mIsFunction : 1;
			mo_bool mIsFunctionBoundary : 1;
			mo_bool mDoesFunctionReturn : 1;
			mo_bool mHasValidStrictMode;
			mo_int32 mLoopDepth;
			mo_int32 mSwitchDepth;
	};

	typedef std::vector<ParserScope> ParserScopeStack;

	struct ParserScopeRef
	{
		public:
			ParserScopeRef(ParserScopeStack* stack, mo_unsigned index)
				: mStack(stack), mIndex(index) { }

			ParserScope* operator->() { return &mStack->at(mIndex); }
			
			mo_unsigned GetIndex() const { return mIndex; }
			mo_bool HasContainingScope() { return ((mIndex > 0) && (!mStack->at(mIndex).IsFunctionBoundary())); }

			ParserScopeRef GetContainingScope()
			{
				MO_ASSERT(HasContainingScope());
				return ParserScopeRef(mStack, mIndex - 1);
			}

		private:
			ParserScopeStack* mStack;
			mo_unsigned mIndex;
	};
}

#endif