---
layout: default
title: "23. Serialization - 5"
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

{% raw %}

* [Get Code](https://github.com/EasyCoding-7/CSharp_Windows_GameServer_Tutorial/tree/Tag-35-PacketGenerator-5)

🍁 현재코드의 문제를 살펴보자.

```csharp
class ClientSession : PacketSession
{
    //  ...

    public override void OnRecvPacket(ArraySegment<byte> buffer)
    {
        int pos = 0;

        ushort size = BitConverter.ToUInt16(buffer.Array, buffer.Offset); 
        pos += 2;
        ushort id = BitConverter.ToUInt16(buffer.Array, buffer.Offset + pos);
        pos += 2;

        // Session에서 Packet 수신시 Switch Case문으로 되어있는데
        // Case가 늘어날수록 부하가 늘어나는 구조이다. -> 개선!
        switch ((PacketID)id)
        {
            case PacketID.PlayerInfoReq:
                {
                    PlayerInfoReq p = new PlayerInfoReq();
                    p.Read(buffer);
                    Console.WriteLine($"PlayerInfoReq: {p.playerId} , PlayerName: {p.name}");

                    foreach(PlayerInfoReq.SkillInfo skill in p.skills)
                    {
                        Console.WriteLine($"SKill: {skill.id}, {skill.level}");
                    }

                }
                break;
```

🍁 그냥 코드를 보는게 나을듯 하여 더이상 정리안함. 코드를 참조할 것.

{% endraw %}