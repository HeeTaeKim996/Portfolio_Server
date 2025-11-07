/* -------------------------------------
	  (1)ServerCoreModel_250527
--------------------------------------*/

#pragma once

class RecvBuffer
{
	enum { BUFFER_COUNT = 10 };	

public:
	RecvBuffer(int32 bufferSize);
	~RecvBuffer();


	BYTE* WritePos() { return &_buffer[_writePos]; }
	BYTE* ReadPos() { return &_buffer[_readPos]; }
	int32 FreeSize() { return _capacity - _writePos; }
	int32 DataSize() { return _writePos - _readPos; }


	void Clean();
	bool OnWrite(int32 numOfBytes);
	bool OnRead(int32 processLen);


private:
	int32 _capacity = 0;

	int32 _bufferSize = 0;
	int32 _readPos = 0;
	int32 _writePos = 0;
	Vector<BYTE> _buffer;
};