#include "pch.h"
/* -------------------------------------
	  (1)ServerCoreModel_250527
--------------------------------------*/
#include "RecvBuffer.h"

RecvBuffer::RecvBuffer(int32 bufferSize) : _bufferSize(bufferSize)
{
	_capacity = bufferSize * BUFFER_COUNT;
	_buffer.resize(_capacity);
}

RecvBuffer::~RecvBuffer()
{
}

void RecvBuffer::Clean()
{
	int32 dataSize = DataSize();
	if (dataSize == 0)
	{
		_writePos = _readPos = 0;
	}
	else
	{
		::memcpy(&_buffer[0], &_buffer[_readPos], dataSize);
		_writePos = dataSize;
		_readPos = 0;
	}
}

bool RecvBuffer::OnRead(int32 processLen)
{
	if (processLen > DataSize())
	{
		return false;
	}

	_readPos += processLen;
	return true;
}

bool RecvBuffer::OnWrite(int32 numOfBytes)
{
	if (FreeSize() < numOfBytes)
	{
		return false;
	}

	_writePos += numOfBytes;
	return true;
}
