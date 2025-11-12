/* -------------------------------------
	  (1)ServerCoreModel_250528
--------------------------------------*/
#pragma once

/*-------------------------
	   FlatBufferReader
-------------------------*/
class FlatBufferReader
{
public:
	FlatBufferReader(BYTE* buffer, uint16 size);
	~FlatBufferReader();

	BYTE* Buffer() { return _buffer; }
	uint16 Size() { return _size; }
	uint16 ReadSize() { return _pos; }
	uint16 FreeSize() { return _size - _pos; }

	template<typename T>
	void Peek(T* dest) { Peek(dest, sizeof(T)); }
	void Peek(void* dest, uint16 len);

	template<typename T>
	T Peek();

	template<typename T>
	void Read(T* dest) { Read(dest, sizeof(T)); }
	void Read(void* dest, uint16 len);

	wstring ReadWString();

	template<typename T>
	T* ZeroCopy();

	template<typename T>
	T Read();

	template<typename T>
	FlatBufferReader& operator >> (OUT T& dest);

	void* AssistCopying(uint16 copyingLen);

private:
	BYTE* _buffer;
	uint16 _size;
	uint16 _pos = 4;
};

template<typename T>
inline T FlatBufferReader::Peek()
{
#ifdef _DEBUG
	if (FreeSize() < sizeof(T))
	{
		CRASH("Crash");
	}
#endif 

	return *reinterpret_cast<T*>(&_buffer[_pos]);
}

template<typename T>
inline T* FlatBufferReader::ZeroCopy()
{
#ifdef _DEBUG
	if (FreeSize() < sizeof(T))
	{
		CRASH("Crash");
	}
#endif 

	T* ptr = reinterpret_cast<T*>(&_buffer[_pos]);
	_pos += sizeof(T);
	return ptr;
}

template<typename T>
inline T FlatBufferReader::Read()
{
#ifdef _DEBUG
	if (FreeSize() < sizeof(T))
	{
		CRASH("Crash");
	}
#endif

	T ret = *reinterpret_cast<T*>(&_buffer[_pos]);
	_pos += sizeof(T);
	return ret;
}

template<typename T>
inline FlatBufferReader& FlatBufferReader::operator>>(OUT T& dest)
{
	using DataType = std::remove_reference_t<T>;
	dest = *reinterpret_cast<DataType*>(&_buffer[_pos]);
	_pos += sizeof(DataType);

	return *this;
}