---
layout: default
title: "[구현] Buffer Helper"
parent: "(C++) 상세 구현"
grand_parent: "Game Server 👾"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get This Code 🌎](https://github.com/EasyCoding-7/Windows_Game_Server_Tutorial/tree/RA-Tag-22)

---

🦄 현재 코드의 개선점에 대해 말해보자.

```cpp
char sendData[1000] = "Hello World";

while (true)
{
    // 패킷을 만들어 넣는 부분인데
    SendBufferRef sendBuffer = GSendBufferManager->Open(4096);

    // 버퍼를 만들고
    BYTE* buffer = sendBuffer->Buffer();

    // 사이즈를 지정하고
    ((PacketHeader*)buffer)->size = (sizeof(sendData) + sizeof(PacketHeader));

    // id를 지정하고
    ((PacketHeader*)buffer)->id = 1; // 1 : Hello Msg

    // 복사하고 등등...

        // 그냥 봐도 버그를 유발할만한 요소가 너무 많다.
    ::memcpy(&buffer[4], sendData, sizeof(sendData));
    sendBuffer->Close((sizeof(sendData) + sizeof(PacketHeader)));

    GSessionManager.Broadcast(sendBuffer);

    this_thread::sleep_for(250ms);
}
```

🦄 최종 목표하는 코드는 아래와 같다

```cpp
virtual int32 OnRecvPacket(BYTE* buffer, int32 len) override
{
    // 패킷이 들어오면
    BufferReader br(buffer, len);

    PacketHeader header;

    // >> 연산을 통해 데이터를 넣을수 있음.
    br >> header;

    uint64 id;
    uint32 hp;
    uint16 attack;

    // 단, 아직도 넣어준 순서 그대로 읽어야한다.
    br >> id >> hp >> attack;

    cout << "ID: " << id << " HP : " << hp << " ATT : " << attack << endl;

    char recvBuffer[4096];
    br.Read(recvBuffer, header.size - sizeof(PacketHeader) - 8 - 4 - 2 
	/* 가변길이인 *char(string) 을 읽기위해서 직접 어디서 부터 읽을지 정해야함. 이후 수정해야할 부분. */);
    cout << recvBuffer << endl;

    return len;
}
```

```cpp
char sendData[1000] = "Hello World";

while (true)
{
    SendBufferRef sendBuffer = GSendBufferManager->Open(4096);

    BufferWriter bw(sendBuffer->Buffer(), 4096);

    PacketHeader* header = bw.Reserve<PacketHeader>();
    // id(uint64), 체력(uint32), 공격력(uint16)
    bw << (uint64)1001 << (uint32)100 << (uint16)10;
    bw.Write(sendData, sizeof(sendData));

    header->size = bw.WriteSize();
    header->id = 1; // 1 : Test Msg

    sendBuffer->Close(bw.WriteSize());

    GSessionManager.Broadcast(sendBuffer);

    this_thread::sleep_for(250ms);
}
```

---

## 구현

🦄 크게 어렵지 않아 자세한 설명은 생략, 그냥 디버깅 돌려보면 이해됨.

```cpp
class BufferWriter
{
public:
	BufferWriter();
	BufferWriter(BYTE* buffer, uint32 size, uint32 pos = 0);
	~BufferWriter();

	BYTE*			Buffer() { return _buffer; }
	uint32			Size() { return _size; }
	uint32			WriteSize() { return _pos; }
	uint32			FreeSize() { return _size - _pos; }

	template<typename T>
	bool			Write(T* src) { return Write(src, sizeof(T)); }
	bool			Write(void* src, uint32 len);

	template<typename T>
	T*				Reserve();

	template<typename T>
	BufferWriter&	operator<<(const T& src);

	template<typename T>
	BufferWriter&	operator<<(T&& src);

private:
	BYTE*			_buffer = nullptr;
	uint32			_size = 0;
	uint32			_pos = 0;
};

template<typename T>
T* BufferWriter::Reserve()
{
	if (FreeSize() < sizeof(T))
		return nullptr;

	T* ret = reinterpret_cast<T*>(&_buffer[_pos]);
	_pos += sizeof(T);
	return ret;
}

template<typename T>
BufferWriter& BufferWriter::operator<<(const T& src)
{
	*reinterpret_cast<T*>(&_buffer[_pos]) = src;
	_pos += sizeof(T);
	return *this;
}

template<typename T>
BufferWriter& BufferWriter::operator<<(T&& src)
{
	*reinterpret_cast<T*>(&_buffer[_pos]) = std::move(src);
	_pos += sizeof(T);
	return *this;
}
```

```cpp
class BufferReader
{
public:
	BufferReader();
	BufferReader(BYTE* buffer, uint32 size, uint32 pos = 0);
	~BufferReader();

	BYTE*			Buffer() { return _buffer; }
	uint32			Size() { return _size; }
	uint32			ReadSize() { return _pos; }
	uint32			FreeSize() { return _size - _pos; }

	template<typename T>
	bool			Peek(T* dest) { return Peek(dest, sizeof(T)); }
	bool			Peek(void* dest, uint32 len);

	template<typename T>
	bool			Read(T* dest) { return Read(dest, sizeof(T)); }
	bool			Read(void* dest, uint32 len);

	template<typename T>
	BufferReader&	operator>>(OUT T& dest);

private:
	BYTE*			_buffer = nullptr;
	uint32			_size = 0;
	uint32			_pos = 0;
};

template<typename T>
inline BufferReader& BufferReader::operator>>(OUT T& dest)
{
	dest = *reinterpret_cast<T*>(&_buffer[_pos]);
	_pos += sizeof(T);
	return *this;
}
```
