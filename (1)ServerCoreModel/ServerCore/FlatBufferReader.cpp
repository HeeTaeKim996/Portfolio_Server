#include "pch.h"
/* -------------------------------------
	  (1)ServerCoreModel_250528
--------------------------------------*/
#include "FlatBufferReader.h"

/*-------------------------
	  FlatBufferReader
-------------------------*/

FlatBufferReader::FlatBufferReader(BYTE* buffer, uint16 size) : _buffer(buffer), _size(size)
{
}

FlatBufferReader::~FlatBufferReader()
{
}

void FlatBufferReader::Peek(void* dest, uint16 len)
{
#ifdef _DEBUG
	if (FreeSize() < len)
	{
		CRASH("할당된 사이즈를 초과하여 읽으려 함");
	}
#endif 
	::memcpy(dest, &_buffer[_pos], len);
}

void FlatBufferReader::Read(void* dest, uint16 len)
{
#ifdef _DEBUG
	if (FreeSize() < len)
	{
		CRASH("할당된 사이즈를 초과하여 읽으려 함");
	}
#endif 
	::memcpy(dest, &_buffer[_pos], len);
	_pos += len;
}

wstring FlatBufferReader::ReadWString()
{
	uint16 byteLen = Read<uint16>();

#ifdef _DEBUG
	if (FreeSize() < byteLen)
	{
		CRASH("할당된 사이즈를 초과하여 읽으려 함");
	}
#endif 

	uint16 wcharLen = byteLen / sizeof(WCHAR);

	const WCHAR* wstPtr = reinterpret_cast<const WCHAR*>(&_buffer[_pos]);
	_pos += byteLen;

	return wstring(wstPtr, wcharLen);
}

void* FlatBufferReader::AssistCopying(uint16 copyingLen)
{
#ifdef _DEBUG
	if (FreeSize() < copyingLen)
	{
		CRASH("할당된 사이즈를 초과하여 읽으려 함");
	}
#endif 

	void* copyingStartPoint = &_buffer[_pos];
	_pos += copyingLen;

	return copyingStartPoint;
}
