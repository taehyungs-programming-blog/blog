---
layout: default
title: "26. "
parent: "(C# IOCP)"
grand_parent: "Game Server 👾"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code](https://github.com/EasyCoding-7/CSharp_Windows_GameServer_Tutorial/tree/Tag-37-chattest-2)

🍁 현재 코드의 문제점

```csharp
public void Broadcast(ClientSession session, string chat)
{
    S_Chat packet = new S_Chat();
    packet.playerId = session.SessionID;

    packet.chat = chat + $"I am {packet.playerId}";

    ArraySegment<byte> segment = packet.Write();

    // 여기서 for문을 돌며 병목현상이 발생.
    lock(_lock)
    {
        foreach(ClientSession sessions in _sessions)
        {
            sessions.Send(segment);
        }
    }
}
```


