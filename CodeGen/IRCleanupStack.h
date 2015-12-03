#ifndef MOENJIN_CODEGEN_IRCLEANUPSTACK_H
#define MOENJIN_CODEGEN_IRCLEANUPSTACK_H

#include "Platform/Configuration.h"
#include "CodeGen/IRCleanup.h"

namespace MoEnjin
{
	class IRCleanupStack
	{
		typedef std::vector<IRCleanup*> IRCleanupCollection;

		public:
			IRCleanupRef begin() const { return IRCleanupRef(this, mCleanups.size()); }
			IRCleanupRef end() const { return IRCleanupRef(this, 0); }

		public:
			IRCleanupStack() { }
			~IRCleanupStack() { }

			mo_uint32 GetCount() const { return mCleanups.size(); }
			IRCleanup* GetAt(mo_uint32 index) const { return mCleanups.at(index); }
			IRCleanupRef GetInnermostCleanup() const { return begin(); }

			mo_bool IsEmpty() const { return mCleanups.empty(); }

			void Pop() { PopImpl(); }

			template<class T>
			void Push(IRCleanupType type)
			{
				IRCleanupTask* task = new T();
				PushImpl(type, task);
			}

			template<class T, class A1>
			void Push(IRCleanupType type, A1 a1)
			{
				IRCleanupTask* task = new T(a1);
				PushImpl(type, task);
			}

			template<class T, class A1, class A2>
			void Push(IRCleanupType type, A1 a1, A2 a2)
			{
				IRCleanupTask* task = new T(a1, a2);
				PushImpl(type, task);
			}

			template<class T, class A1, class A2, class A3>
			void Push(IRCleanupType type, A1 a1, A2 a2, A3 a3)
			{
				IRCleanupTask* task = new T(a1, a2, a3);
				PushImpl(type, task);
			}

			template<class T, class A1, class A2, class A3, class A4>
			void Push(IRCleanupType type, A1 a1, A2 a2, A3 a3, A4 a4)
			{
				IRCleanupTask* task = new T(a1, a2, a3, a4);
				PushImpl(type, task);
			}

			template<class T, class A1, class A2, class A3, class A4, class A5>
			void Push(IRCleanupType type, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5)
			{
				IRCleanupTask* task = new T(a1, a2, a3, a4, a5);
				PushImpl(type, task);
			}

		private:
			void PushImpl(IRCleanupType type, IRCleanupTask* task)
			{
				IRCleanupRef enclosingCleanup = begin();
				IRCleanup* cleanup = new IRCleanup(type, task, enclosingCleanup);

				mCleanups.push_back(cleanup);
			}

			void PopImpl()
			{
				MO_ASSERT(!mCleanups.empty() && "popping an empty exception stack.");

				IRCleanup* cleanup = mCleanups.back();
				mCleanups.pop_back();

				delete cleanup;
			}

			IRCleanupCollection mCleanups;
	};
}

#endif