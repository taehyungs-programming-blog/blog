---
layout: default
title: "[구현] JobQueue - 1"
parent: "(C++) 상세 구현"
grand_parent: "Game Server 👾"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get This Code 🌎](https://github.com/EasyCoding-7/Windows_Game_Server_Tutorial/tree/RA-Tag-29)

---

## JobQueue의 필요성?

👀 예를 들어 설명하자면 Client로 부터 온 메시지를 아래 처럼 브로드캐스팅 해준다고 가정해 보자.<br>
👀 Client가 하나면 모르겠는데 100명 1000명 이라면? 모두 저 **Broadcast에서 lock**을 잡히게 된다.<br>
👀 특히 lock이 spin-lock이라면 모든 **thread가 무한 대기**를 하게 되는데 ... 이게 과연 효율적인 방법일까?

```cpp
bool Handle_C_CHAT(PacketSessionRef& session, Protocol::C_CHAT& pkt)
{
	std::cout << pkt.msg() << endl;

	Protocol::S_CHAT chatPkt;
	chatPkt.set_msg(pkt.msg());
	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(chatPkt);

	GRoom.Broadcast(sendBuffer); // 아래참고

	return true;
}
```

```cpp
void Room::Broadcast(SendBufferRef sendBuffer)
{
    // 일단 락을 잡고
    WRITE_LOCK;
    for (auto& p : _players)
    {
        // 다 돌때까지 기다려야 한다.
        p.second->ownerSession->Send(sendBuffer);
    }
}
```

---

## JobQueue로 해결해보자

👀 수신측은 대략 이렇게 처리예정<br>
👀 우선은 가장 원시적인 방법이고 차근차근 개선예정

```cpp
int main()
{
    // ...

    // main thread에서 job을 flush
	while (true)
	{
		GRoom.FlushJob();   // 아래 참고

		this_thread::sleep_for(1ms);
	}
```

```cpp
void Room::FlushJob()
{
    while (true)
    {
        // _jobs가 있는동안 main thread에서 잡고있는다.
        JobRef job = _jobs.Pop();
        if (job == nullptr)
            break;

        job->Execute();
    }
}
```

```cpp
bool Handle_C_CHAT(PacketSessionRef& session, Protocol::C_CHAT& pkt)
{
	std::cout << pkt.msg() << endl;

	Protocol::S_CHAT chatPkt;
	chatPkt.set_msg(pkt.msg());
	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(chatPkt);

    // Job은 패킷이 수신시 만들어 진다.
	GRoom.PushJob(MakeShared<BroadcastJob>(GRoom, sendBuffer));
    /*
        class BroadcastJob : public IJob
        {
        public:
            BroadcastJob(Room& room, SendBufferRef sendBuffer) : _room(room), _sendBuffer(sendBuffer)
            {
            }

            virtual void Execute() override
            {
                _room.Broadcast(_sendBuffer);
            }

        public:
            Room& _room;
            SendBufferRef _sendBuffer;
        };
    */

	return true;
}
```

```cpp
// Enter, Leave, BroadCast를 모두 Single Thread로 작업하듯 Lock없이 가능

void Room::Enter(PlayerRef player)
{
	_players[player->playerId] = player;
}

void Room::Leave(PlayerRef player)
{
	_players.erase(player->playerId);
}

void Room::Broadcast(SendBufferRef sendBuffer)
{
	for (auto& p : _players)
	{
		p.second->ownerSession->Send(sendBuffer);
	}
}
```

👀 현재 코드의 문제는 Job이 생성될때마다 클래스를 새로 생성해야 한다.

```cpp
// 매번 이런걸 만들어야 한다는 말...
class BroadcastJob : public IJob
{
public:
	BroadcastJob(Room& room, SendBufferRef sendBuffer) : _room(room), _sendBuffer(sendBuffer)
	{
	}

	virtual void Execute() override
	{
		_room.Broadcast(_sendBuffer);
	}

public:
	Room& _room;
	SendBufferRef _sendBuffer;
};
```

👀 아직까지 문제는 **Job을 매번 만들어야 하며**<br>
👀 main thread(특정 thread)에서 job이 **모두 처리될때**까지 돌아야 한다

---
---

👀 복습) 송신측

```cpp
template<typename T>
static SendBufferRef MakeSendBuffer(T& pkt, uint16 pktId)
{
    const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
    const uint16 packetSize = dataSize + sizeof(PacketHeader);

    // SendBuffer를 Open
    SendBufferRef sendBuffer = GSendBufferManager->Open(packetSize);
    /*
        SendBufferRef SendBufferManager::Open(uint32 size)
        {
            if (LSendBufferChunk == nullptr)
            {
                LSendBufferChunk = Pop(); // WRITE_LOCK
                LSendBufferChunk->Reset();
            }		

            ASSERT_CRASH(LSendBufferChunk->IsOpen() == false);

            // 다 썼으면 버리고 새거로 교체
            if (LSendBufferChunk->FreeSize() < size)
            {
                LSendBufferChunk = Pop(); // WRITE_LOCK
                LSendBufferChunk->Reset();
            }

            return LSendBufferChunk->Open(size);
        }
    */
```

```cpp
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

    // using SendBufferChunkRef	= std::shared_ptr<class SendBufferChunk>;
    // 생성자 -> xnew<SendBufferChunk>() / 소멸자 -> PushGlobal
        // 소멸시 메모리를 다시 buffer에 넣음
	return SendBufferChunkRef(xnew<SendBufferChunk>(), PushGlobal);
}
```