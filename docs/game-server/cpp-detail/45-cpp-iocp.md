---
layout: default
title: "[구현] PacketHandler"
parent: "(C++) 상세 구현"
grand_parent: "Game Server 👾"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get This Code 🌎](https://github.com/EasyCoding-7/Windows_Game_Server_Tutorial/tree/RA-Tag-23)

---

🦄 지금 코드의 문제를 보자면 ...

```cpp
virtual int32 OnRecvPacket(BYTE* buffer, int32 len) override
{
    // 문제1) 어떤 패킷이 들어왔는지 header를 까야 알수 있음.
    BufferReader br(buffer, len);

    PacketHeader header;
    br >> header;

    uint64 id;
    uint32 hp;
    uint16 attack;

    // 문제2) 넣어준 순서 그대로 읽어야함.
    br >> id >> hp >> attack;

    cout << "ID: " << id << " HP : " << hp << " ATT : " << attack << endl;

    char recvBuffer[4096];
    br.Read(recvBuffer, header.size - sizeof(PacketHeader) - 8 - 4 - 2);
    cout << recvBuffer << endl;

    return len;
}
```

🦄 개선해보자. 역시 어렵지 않기에 설명은 별도로 없음.

---

```cpp
class ServerSession : public PacketSession
{
public:
	~ServerSession()
	{
		cout << "~ServerSession" << endl;
	}

	virtual void OnConnected() override
	{
		//cout << "Connected To Server" << endl;
	}

	virtual void OnRecvPacket(BYTE* buffer, int32 len) override
	{
        // 패킷이 들어올 경우 Handler에게 보내고
		ClientPacketHandler::HandlePacket(buffer, len);
	}

	virtual void OnSend(int32 len) override
	{
		//cout << "OnSend Len = " << len << endl;
	}

	virtual void OnDisconnected() override
	{
		//cout << "Disconnected" << endl;
	}
};
```

```cpp
void ClientPacketHandler::HandlePacket(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PacketHeader header;
	br >> header;

	switch (header.id)
	{
    // 아직은 예제라 한개의 패킷만 처리하게 만듦.
	case S_TEST:
		Handle_S_TEST(buffer, len);
		break;
	}
}

void ClientPacketHandler::Handle_S_TEST(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PacketHeader header;
	br >> header;

	uint64 id;
	uint32 hp;
	uint16 attack;
	br >> id >> hp >> attack;

	cout << "ID: " << id << " HP : " << hp << " ATT : " << attack << endl;

    // 가변데이터는 이런식으로 받는다
	vector<BuffData> buffs;
	uint16 buffCount;
	br >> buffCount;

	buffs.resize(buffCount);
	for (int32 i = 0; i < buffCount; i++)
	{
		br >> buffs[i].buffId >> buffs[i].remainTime;
	}

	cout << "BufCount : " << buffCount << endl;
	for (int32 i = 0; i < buffCount; i++)
	{
		cout << "BufInfo : " << buffs[i].buffId << " " << buffs[i].remainTime << endl;
	}
}
```

---

```cpp
// 서버도 마찬가지로
char sendData[1000] = "Hello World";

while (true)
{
    vector<BuffData> buffs{ BuffData {100, 1.5f}, BuffData{200, 2.3f}, BuffData {300, 0.7f } };
    SendBufferRef sendBuffer = ServerPacketHandler::Make_S_TEST(1001, 100, 10, buffs);
    GSessionManager.Broadcast(sendBuffer);

    this_thread::sleep_for(250ms);
}
```

---

🦄 헷갈리는 부분 복습 👉 **어떻게 OnRecvPacket까지 오게되는가?**

```cpp
void Session::RegisterRecv()
{
	// Completion Port를 채우고
	if (SOCKET_ERROR == ::WSARecv(_socket, &wsaBuf, 1, OUT &numOfBytes, OUT &flags, &_recvEvent, nullptr))
```

```cpp
bool IocpCore::Dispatch(uint32 timeoutMs)
{
	DWORD numOfBytes = 0;
	ULONG_PTR key = 0;
	IocpEvent* iocpEvent = nullptr;

    // Completion Port의 응답을 받으면, IocpObject의 Dispatch를 호출
	if (::GetQueuedCompletionStatus(_iocpHandle, OUT & numOfBytes, OUT & key, OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timeoutMs))
	{
		IocpObjectRef iocpObject = iocpEvent->owner;
		iocpObject->Dispatch(iocpEvent, numOfBytes);
	}
```

```cpp
void Session::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	switch (iocpEvent->eventType)
	{
	case EventType::Connect:
		ProcessConnect();
		break;
	case EventType::Disconnect:
		ProcessDisconnect();
		break;
	case EventType::Recv:
		ProcessRecv(numOfBytes);
```

```cpp
void Session::ProcessRecv(int32 numOfBytes)
{
	_recvEvent.owner = nullptr; // RELEASE_REF

	if (numOfBytes == 0)
	{
		Disconnect(L"Recv 0");
		return;
	}

	if (_recvBuffer.OnWrite(numOfBytes) == false)
	{
		Disconnect(L"OnWrite Overflow");
		return;
	}

	int32 dataSize = _recvBuffer.DataSize();
	int32 processLen = OnRecv(_recvBuffer.ReadPos(), dataSize); // 컨텐츠 코드에서 재정의
```