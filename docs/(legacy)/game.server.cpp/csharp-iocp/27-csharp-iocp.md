---
layout: default
title: "27. Packet 모아보내기"
parent: "(C# IOCP)"
grand_parent: "Game Server 👾"
great_grand_parent: "Legacy Archive"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code](https://github.com/EasyCoding-7/CSharp_Windows_GameServer_Tutorial/tree/Tag-40-packetsum)

🍁 현재코드의 문제

```csharp
public void Broadcast(ClientSession session, string chat)
{
    S_Chat packet = new S_Chat();
    packet.playerId = session.SessionID;

    packet.chat = chat + $"I am {packet.playerId}";

    ArraySegment<byte> segment = packet.Write();

    // 매번 Send를 하는데 이걸 좀 모아서 보내면 어떨까?
    foreach(ClientSession sessions in _sessions)
    {
        sessions.Send(segment);
    }
}
```

```csharp
// 물론 기억하다 싶이, Engine내부적으로 Packet을 Queue에 넣어 보내긴한다.
public void Send(ArraySegment<byte> sendBuff)
{
    lock (_lock)
    {
        _sendQueue.Enqueue(sendBuff);
        if (_pendingList.Count == 0)
            RegisterSend();
    }
}

// 컨텐츠단에서도 조금 모아서 Queue에 보내게 해보자.
```

---

구현

```csharp
// ...

static void Main(string[] args)
{
    // ...

    while (true)
    {
        // 이제 main thread에서 packet을 모았다 Flush하게 할 예정
        Room.Push(() => Room.Flush());
        Thread.Sleep(250);
    }
}
```

```csharp
class GameRoom : IJobQueue
{
    List<ClientSession> _sessions = new List<ClientSession>();
    JobQueue _jobQueue = new JobQueue();

    // List로 보낼 패킷을 관리
    List<ArraySegment<byte>> _pendingList = new List<ArraySegment<byte>>();

    // ...

    public void Flush()
    {
        foreach (ClientSession sessions in _sessions)
        {
            sessions.Send(_pendingList);
        }

        Console.WriteLine($"Flushed {_pendingList.Count} items");
        _pendingList.Clear();
    }
```

