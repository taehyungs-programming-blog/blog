---
layout: default
title: "14. Serialization"
parent: "(기초)"
grand_parent: "(GameServer C# 🎯)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code](https://github.com/Arthur880708/CSharp_GameServer_Basic_Examples/tree/Tag-26-Serialization-1)

## Step.1 - Serialization(직렬화)란 뭘까? 

* 사실 지금도 하고 있었다

```csharp
public override void OnConnected(EndPoint endPoint)
{
	Console.WriteLine($"[OnConnected] {endPoint}");

	Packet packet = new Packet() { size = 4, packetId = 7 };

	// 보내는 부분
	for(int i = 0; i < 5; i++)
	{
		ArraySegment<byte> openSegment = SendBufferHelper.Open(4096);
		byte[] buffer = BitConverter.GetBytes(packet.size);
		byte[] buffer2 = BitConverter.GetBytes(packet.packetId);

		// packet이라는 오브젝트의 데이터를 byte 버퍼에 밀어 넣는데 이를 직렬화라 한다.
		// 이게 별거라고?? -> 만약 2차원배열, 힙영역의 데이터일 경우 또 말이 달라짐
		Array.Copy(buffer, 0, openSegment.Array, openSegment.Offset, buffer.Length);
		Array.Copy(buffer2, 0, openSegment.Array, openSegment.Offset + buffer.Length, buffer2.Length);
		ArraySegment<byte> sendBuffer = SendBufferHelper.Close(packet.size);

		Send(sendBuffer);
	}
```

* 반대로 직렬화된 데이터를 다시 오브젝트 형식으로 만드는것을 역직렬화라고 한다

```csharp
public override void OnRecvPacket(ArraySegment<byte> buffer)
{
	ushort size = BitConverter.ToUInt16(buffer.Array, buffer.Offset);
	ushort id = BitConverter.ToUInt16(buffer.Array, buffer.Offset + 2);
	Console.WriteLine($"RecvPacketID : {id}, Size {size}");
}
```

* 기존에 사용하고 있던 패킷정보를 좀 현실적으로 정리해 보자

```csharp
class Packet
{
	public ushort size;
	public ushort packetId;
}

public enum PacketID
{
	PlayerInfoReq = 1,
	PlayerInfoOk = 2,
}

class PlayerInfoReq : Packet
{
	public long playerId;
}

class PlayerInfoOk : Packet
{
	public int hp;
	public int attack;
}
```

* 보내는쪽의 실제 사용은 이렇게 한다

```csharp
public override void OnConnected(EndPoint endPoint)
{
	Console.WriteLine($"[OnConnected] {endPoint}");

	PlayerInfoReq packet = new PlayerInfoReq() { size = 4, packetId = (ushort)PacketID.PlayerInfoReq, playerId = 1001 };

	// 보내는 부분
	//for (int i = 0; i < 5; i++)
	{
		ArraySegment<byte> openSegment = SendBufferHelper.Open(4096);

		byte[] size = BitConverter.GetBytes(packet.size);
		byte[] packetId = BitConverter.GetBytes(packet.packetId);
		byte[] playerId = BitConverter.GetBytes(packet.playerId);

		// 이 부분 실수의 여지가 많기에 개선이 필요함.
		ushort count = 0;
		Array.Copy(size, 0, openSegment.Array, openSegment.Offset, 2);
		count += 2;
		Array.Copy(packetId, 0, openSegment.Array, openSegment.Offset + count, 2);
		count += 2;
		Array.Copy(playerId, 0, openSegment.Array, openSegment.Offset + count, 8);
		count += 8;

		ArraySegment<byte> sendBuffer = SendBufferHelper.Close(count);

		Send(sendBuffer);
	}
}
```

* 받는쪽의 실제사용

```csharp
public override void OnRecvPacket(ArraySegment<byte> buffer)
{
	ushort count = 0;
	ushort size = BitConverter.ToUInt16(buffer.Array, buffer.Offset);
	count += 2;
	ushort id = BitConverter.ToUInt16(buffer.Array, buffer.Offset + count);
	count += 2;

	switch((PacketID)id)
	{
		case PacketID.PlayerInfoReq:
			{
				long playerId = BitConverter.ToInt64(buffer.Array, buffer.Offset + count);
				count += 8;
				Console.WriteLine($"PlayerInfoReq : {playerId}");
			}
			break;
	}
}
```

---

## 개선사항

```csharp
public override void OnConnected(EndPoint endPoint)
{
	// ...

		ArraySegment<byte> openSegment = SendBufferHelper.Open(4096);

		// GetBytes동작을 하며 내부적으로 byte[] = new를 할텐데 이 부분이 부담스럽다
		byte[] size = BitConverter.GetBytes(packet.size);
		byte[] packetId = BitConverter.GetBytes(packet.packetId);
		byte[] playerId = BitConverter.GetBytes(packet.playerId);
```

```csharp
// 버퍼로 바로 넣는 방법도 존재
public override void OnConnected(EndPoint endPoint)
{
	Console.WriteLine($"OnConnected : {endPoint}");

	PlayerInfoReq packet = new PlayerInfoReq() { size = 4, packetId = (ushort)PacketID.PlayerInfoReq, playerId = 1001 };


	// 보낸다
	for (int i = 0; i < 5; i++)
	{
		ArraySegment<byte> s = SendBufferHelper.Open(4096);
		//byte[] size = BitConverter.GetBytes(packet.size);
		//byte[] packetId = BitConverter.GetBytes(packet.packetId);
		//byte[] playerId = BitConverter.GetBytes(packet.playerId);

		ushort size = 0;
		bool success = true;
		size += 2;
		success &= BitConverter.TryWriteBytes(new Span<byte>(s.Array, s.Offset + size, s.Count - size), packet.packetId);
		size += 2;
		success &= BitConverter.TryWriteBytes(new Span<byte>(s.Array, s.Offset + size, s.Count - size), packet.playerId);
		size += 8;
		success &= BitConverter.TryWriteBytes(new Span<byte>(s.Array, s.Offset, s.Count), size);

		ArraySegment<byte> sendBuff = SendBufferHelper.Close(size);

		if (success)
			Send(sendBuff);
	}
}
```

* 그런데 .NET Core에서는 `TryWriteBytes`를 지원하는데 .NET Frame(과거버전)에서 지원이 되게 만들어 보면

```csharp
class ServerSession : Session
{
	// unsafe는 포인터를 쓰겠다는 선언
	static unsafe void ToBytes(byte[] array, int offset, ulong value)
	{
		fixed (byte* ptr = &array[offset])
			*(ulong*)ptr = value;
	}

	static unsafe void ToBytes<T>(byte[] array, int offset, T value) where T : unmanaged
	{
		fixed (byte* ptr = &array[offset])
			*(T*)ptr = value;
	}
```

* 하지만 `TryWriteBytes`를 쓰는쪽으로 하자(아무래도 안전하다)