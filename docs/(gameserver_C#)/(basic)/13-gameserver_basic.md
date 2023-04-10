---
layout: default
title: "13. PacketSession"
parent: "(기초)"
grand_parent: "(GameServer C# 🎯)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code](https://github.com/Arthur880708/CSharp_GameServer_Basic_Examples/tree/Tag-25-PacketSession)

* PacketSession을 만들어보자.
* Sender를 좀더 쉽게 쓰기위핸 Wrapper라 생각하자.


```csharp
public abstract class PacketSession : Session
{
    public static readonly short HeaderSize = 2;

    public sealed override int OnRecv(ArraySegment<byte> buffer)
    {
        int processLen = 0;

        while(true)
        {
            // 헤더파싱
            if(buffer.Count < HeaderSize)
                break;

            ushort dataSize = BitConverter.ToUInt16(buffer.Array, buffer.Offset);
            if (buffer.Count < dataSize)
                break;

            OnRecvPacket(new ArraySegment<byte>(buffer.Array, buffer.Offset, dataSize));

            processLen += dataSize;
            buffer = new ArraySegment<byte>(buffer.Array, buffer.Offset + dataSize, buffer.Count - dataSize);
        }

        return processLen;
    }

    public abstract void OnRecvPacket(ArraySegment<byte> buffer);
}
```

```csharp
// DummyClient
// Program.cs

class GameSession : PacketSession
{
    public override void OnRecvPacket(ArraySegment<byte> buffer)
    {
        ushort size = BitConverter.ToUInt16(buffer.Array, buffer.Offset);
        ushort id = BitConverter.ToUInt16(buffer.Array, buffer.Offset + 2);
        Console.WriteLine($"RecvPacketID : {id}, Size {size}");
    }
```