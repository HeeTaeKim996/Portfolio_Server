#include "pch.h"
/* -------------------------------------
	  (1)ServerCoreModel_250527
--------------------------------------*/
#include "SendBuffer.h"

/*--------------------
	   SendBuffer
--------------------*/


SendBuffer::SendBuffer(SendBufferChunkRef owner, BYTE* buffer, uint32 allocSize)
	: _owner(owner), _buffer(buffer), _allocSize(allocSize)
{
}

SendBuffer::~SendBuffer()
{
}

void SendBuffer::Close(uint32 writeSize)
{
	ASSERT_CRASH(_allocSize >= writeSize);
	_writeSize = writeSize;
	_owner->Close(writeSize);
}


/*--------------------------
	   SendBufferChunk
--------------------------*/

SendBufferChunk::SendBufferChunk()
{
}

SendBufferChunk::~SendBufferChunk()
{
	{ // DEBUG
		//cout << "SendBuffer.cpp__DEBUG : SendBufferChunk " << endl;
	}
}

void SendBufferChunk::Reset()
{
	_open = false;
	_usedSize = 0;
}

SendBufferRef SendBufferChunk::Open(uint32 allocSize)
{
#ifdef _DEBUG
	ASSERT_CRASH(allocSize <= SEND_BUFFER_CHUNK_SIZE);
#endif 
	ASSERT_CRASH(_open == false);

	if (allocSize > FreeSize())
	{
		return nullptr;
	}

	_open = true;
	return ObjectPool<SendBuffer>::MakeShared(shared_from_this(), &_buffer[_usedSize], allocSize);
}

void SendBufferChunk::Close(uint32 writeSize)
{
	ASSERT_CRASH(_open == true);
	_open = false;
	_usedSize += writeSize;
}


/*--------------------------
	  SendBufferManager
--------------------------*/

SendBufferRef SendBufferManager::MakeSendBuff()
{
	return Open(1024);
}

SendBufferRef SendBufferManager::Open(uint32 size)
{																				
	if (LSendBufferChunk == nullptr)
	{
		LSendBufferChunk = Pop(); // WRITE_LOCK
	}

	ASSERT_CRASH(LSendBufferChunk->IsOpen() == false);

	if (LSendBufferChunk->FreeSize() < size)
	{
		LSendBufferChunk = Pop(); // WRITE_LOCK
		LSendBufferChunk->Reset();
	}

	return LSendBufferChunk->Open(size);
}

SendBufferChunkRef SendBufferManager::Pop()
{
	{
		WRITE_LOCK;
		if (_sendBufferChunks.empty() == false)
		{
			SendBufferChunkRef sendBufferChunk = _sendBufferChunks.back();
			_sendBufferChunks.pop_back();
			return sendBufferChunk;
		}
	}

	return SendBufferChunkRef(xnew<SendBufferChunk>(), PushGlobal);
}

void SendBufferManager::Push(SendBufferChunkRef buffer)
{
	WRITE_LOCK;
	_sendBufferChunks.push_back(buffer);
}

void SendBufferManager::PushGlobal(SendBufferChunk* buffer)
{
	GSendBufferManager->Push(SendBufferChunkRef(buffer, PushGlobal));

	

#ifdef _DEBUG
	//cout << "SendBuffer.cpp__DEBUG : SendBufferChunk chunks  push_back " << endl;
#endif // _DEBUG
}
