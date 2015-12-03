#include "Core/TeeStream.h"

namespace MoEnjin
{
	TeeStreamBuffer::TeeStreamBuffer()
		: mInStream(0) { }

	TeeStreamBuffer::TeeStreamBuffer(std::istream& istr)
		: mInStream(&istr) { }

	TeeStreamBuffer::TeeStreamBuffer(std::ostream& ostr)
		: mInStream(0)
	{
		mStreams.push_back(&ostr);
	}

	TeeStreamBuffer::~TeeStreamBuffer() { }

	void TeeStreamBuffer::Add(std::ostream& ostr)
	{
		mStreams.push_back(&ostr);
	}

	mo_int32 TeeStreamBuffer::Read()
	{
		if (mInStream)
		{
			mo_int32 c = mInStream->get();

			if (c != -1) 
				Write((mo_utf8char)c);

			return c;
		}

		return -1;
	}

	mo_int32 TeeStreamBuffer::Write(mo_utf8char c)
	{
		for (StreamCollection::iterator it = mStreams.begin(); it != mStreams.end(); ++it)
			(*it)->put(c);

		return ToInt(c);
	}



	TeeStreamIOS::TeeStreamIOS()
	{
		init(&mBuf);
	}

	TeeStreamIOS::TeeStreamIOS(std::istream& istr)
		: mBuf(istr)
	{
		init(&mBuf);
	}

	TeeStreamIOS::TeeStreamIOS(std::ostream& ostr)
		: mBuf(ostr)
	{
		init(&mBuf);
	}

	TeeStreamIOS::~TeeStreamIOS() { }

	void TeeStreamIOS::Add(std::ostream& ostr)
	{
		mBuf.Add(ostr);
	}


	TeeStreamBuffer* TeeStreamIOS::rdbuf()
	{
		return &mBuf;
	}

	TeeInputStream::TeeInputStream(std::istream& istr)
		: TeeStreamIOS(istr)
		, std::istream(&mBuf) { }


	TeeInputStream::~TeeInputStream() { }


	TeeOutputStream::TeeOutputStream()
		: std::ostream(&mBuf) { }


	TeeOutputStream::TeeOutputStream(std::ostream& ostr)
		: TeeStreamIOS(ostr)
		, std::ostream(&mBuf) { }

	TeeOutputStream::~TeeOutputStream() { }
}