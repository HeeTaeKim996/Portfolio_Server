#include "pch.h"
/* -------------------------------------
	  (1)ServerCoreModel_250527
--------------------------------------*/
#include "FlatBufferWriter.h"
#include "FlatBufferReader.h"
#include "SendBuffer.h"

/*-------------------------
	  FlatBufferWriter
-------------------------*/

FlatBufferWriter::FlatBufferWriter(SendBufferRef& sendBufferRef, uint16 id)
	: _buffer(sendBufferRef->Buffer()), _sendBufferRef(sendBufferRef), _size(sendBufferRef->AllocSize())
{
	_header = reinterpret_cast<PacketHeader*>(_buffer);
	_header->id = id;
}

FlatBufferWriter::~FlatBufferWriter()
{
	_header->size = _pos;

	_sendBufferRef->Close(_pos);
}

void FlatBufferWriter::Write(void* src, uint16 len)
{
	if (FreeSize() < len)
	{
#ifdef _DEBUG
		assert(0);
#endif 
	}

	::memcpy(&_buffer[_pos], src, len);
	_pos += len;
}

void FlatBufferWriter::WriteWString(const wstring& wstr)
{
	uint16 len = static_cast<uint16>(wstr.size());
	uint16 byteLen = sizeof(WCHAR) * len;

	if (FreeSize() < sizeof(uint16) + byteLen)
	{
#ifdef _DEBUG
		assert(0);
#endif 
	}

	*reinterpret_cast<uint16*>(&_buffer[_pos]) = byteLen;
	_pos += sizeof(uint16);

	::memcpy(&_buffer[_pos], wstr.data(), byteLen);
	_pos += byteLen;
}

void FlatBufferWriter::CopyFBR(FlatBufferReader& fbr, uint16 copyingLen)
{
#ifdef _DEBUG
	if (FreeSize() < copyingLen)
	{
		assert(0);
	}
#endif 
	::memcpy(&_buffer[_pos], fbr.AssistCopying(copyingLen), copyingLen);

	_pos += copyingLen;
}



/*----------------------------
		FBW_SmallHeader
----------------------------*/

FBW_SmallHeader::FBW_SmallHeader(FlatBufferWriter& fbw) : _fbw(fbw)
{
	_smallHeader = fbw.Reserve<uint16>();
	_fbwStartPos = fbw.WriteSize();
}

FBW_SmallHeader::~FBW_SmallHeader()
{
	*_smallHeader = _fbw.WriteSize() - _fbwStartPos;
}
