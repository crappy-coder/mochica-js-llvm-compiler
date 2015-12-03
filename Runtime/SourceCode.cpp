#include "SourceCode.h"
#include <Poco/FileStream.h>
#include <Poco/StreamCopier.h>

namespace MoEnjin
{
	SourceCode::SourceCode()
		: mSource(mo_null)
		, mUrl()
		, mOffsetBegin(0)
		, mOffsetEnd(0)
		, mStartLine(0) { }

	SourceCode::SourceCode(const UTF8String& source, const UTF8String& url, mo_int32 startLine)
		: mSource(source)
		, mUrl(url)
		, mOffsetBegin(0)
		, mOffsetEnd(source.GetLength())
		, mStartLine(MO_MAX(startLine, 1)) { }

	SourceCode::SourceCode(const UTF8String& source, const UTF8String& url, mo_int32 begin, mo_int32 end, mo_int32 startLine)
		: mSource(source)
		, mUrl(url)
		, mOffsetBegin(begin)
		, mOffsetEnd(end)
		, mStartLine(MO_MAX(startLine, 1)) { }

	UTF8String SourceCode::ToString() const
	{
		if(mSource.IsNull())
			return UTF8String();

		return mSource.Substring(mOffsetBegin, mOffsetEnd - mOffsetBegin);
	}

	UTF8String SourceCode::ToString(mo_int32 begin, mo_int32 end) const
	{
		if(mSource.IsNull())
			return UTF8String();

		return mSource.Substring(begin, end - begin);
	}

	SourceCode SourceCode::FromFile(const UTF8String& url)
	{
		mo_utf8string source;
		Poco::FileStream fs(url.Get());
		Poco::StreamCopier::copyToString(fs, source);

		fs.close();

		return SourceCode(source, url);
	}
}