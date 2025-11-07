/* -------------------------------------
	  (1)ServerCoreModel_250527
--------------------------------------*/

#pragma once

class FlatBufferReader;

/*-------------------------
  	  FlatBufferWriter
-------------------------*/
class FlatBufferWriter
{
public:
	FlatBufferWriter(SendBufferRef& sendBufferRef, uint16 id);
	~FlatBufferWriter();
	BYTE* Buffer() { return _buffer; }
	uint16 Size() { return _size; }
	uint16 WriteSize() { return _pos; }
	uint16 FreeSize() { return _size - _pos; }

	template<typename T>
	void Write(T* src) { Write(src, sizeof(T)); }
	void Write(void* src, uint16 len);

	template<typename T, typename... Args>
	T* Insert_ZeroCopy(Args&&... args);

	template<typename T>
	void Write(T value);

	void WriteWString(const wstring& wstr);

	template<typename T>
	T* Reserve();

	template<typename T>
	FlatBufferWriter& operator << (T&& src);

	void CopyFBR(FlatBufferReader& fbr, uint16 copyingLen);


private:
	BYTE* _buffer;
	uint16 _size;
	uint16 _pos = 4; // �� PacketHeader �� size�� 4�� ����
	PacketHeader* _header;
	SendBufferRef& _sendBufferRef;
};

template<typename T, typename... Args>
inline T* FlatBufferWriter::Insert_ZeroCopy(Args&&... args)
{
#ifdef _DEBUG
	if (FreeSize() < sizeof(T))
	{
		assert(0);
	}
#endif 
	
	T* ptr = reinterpret_cast<T*>(&_buffer[_pos]);
	new(ptr) T(std::forward<Args>(args)...);

	_pos += sizeof(T);

	return ptr;
}

template<typename T>
inline void FlatBufferWriter::Write(T value)
{
#ifdef _DEBUG
	if (FreeSize() < sizeof(T))
	{
		assert(0);
	}
#endif 

	::memcpy(&_buffer[_pos], &value, sizeof(T));

	//*reinterpret_cast<T*>(&_buffer[_pos]) = value;

	_pos += sizeof(T);
}

template<typename T>
inline T* FlatBufferWriter::Reserve()
{
#ifdef _DEBUG
	if (FreeSize() < sizeof(T))
	{
		assert(0);
	}
#endif 

	T* ret = reinterpret_cast<T*>(&_buffer[_pos]);
	_pos += sizeof(T);

	return ret;
}

template<typename T>
inline FlatBufferWriter& FlatBufferWriter::operator << (T&& src)
{
	using DataType = std::remove_reference_t<T>;
	*reinterpret_cast<DataType*>(&_buffer[_pos]) = std::forward<DataType>(src);
	_pos += sizeof(T);

	return *this;
}


/*----------------------------
		FBW_SmallHeader
----------------------------*/
class FBW_SmallHeader
{
public:
	FBW_SmallHeader(FlatBufferWriter& fbw);
	~FBW_SmallHeader();

private:
	FlatBufferWriter& _fbw;
	uint16* _smallHeader;
	uint16 _fbwStartPos;
};