#ifndef MOENJIN_RUNTIME_SOURCECODE_H
#define MOENJIN_RUNTIME_SOURCECODE_H

#include "Platform/Configuration.h"

namespace MoEnjin
{
	class UTF8String;
	class SourceCode
	{
		public:
			SourceCode();
			SourceCode(const UTF8String& source, const UTF8String& url, mo_int32 startLine = 1);
			SourceCode(const UTF8String& source, const UTF8String& url, mo_int32 begin, mo_int32 end, mo_int32 startLine);

			const UTF8String& GetSource() const { return mSource; }
			const UTF8String& GetUrl() const { return mUrl; }
			const mo_int32 GetLength() const { return (mOffsetEnd - mOffsetBegin); }
			const mo_int32 GetBeginOffset() const { return mOffsetBegin; }
			const mo_int32 GetEndOffset() const { return mOffsetEnd; }
			const mo_int32 GetStartLine() const { return mStartLine; }

			const mo_bool IsNull() const { return mSource.IsNull(); }

			UTF8String ToString() const;
			UTF8String ToString(mo_int32 begin, mo_int32 end) const;

			SourceCode CreateExpression(mo_uint32 openBrace, mo_uint32 closeBrace, mo_int32 startLine);

			static SourceCode FromFile(const UTF8String& url);

		private:
			UTF8String mSource;
			UTF8String mUrl;
			int mOffsetBegin;
			int mOffsetEnd;
			int mStartLine;
	};

	mo_inline SourceCode SourceCode::CreateExpression(mo_uint32 openBrace, mo_uint32 closeBrace, mo_int32 startLine)
	{
		MO_ASSERT(mSource[openBrace] == '{');
		MO_ASSERT(mSource[closeBrace] == '}');

		return SourceCode(mSource, mUrl, openBrace, closeBrace + 1, startLine);
	}
}

#endif