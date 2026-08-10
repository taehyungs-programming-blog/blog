---
layout: default
title: "9. ProtoBuf"
parent: "(Advanced)"
grand_parent: "(Unity ✨)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/EasyCoding-7/UnityPortfolio/tree/9.Protobuf)

---

* [ProtoBuf C# Tutorial 🌍](https://developers.google.com/protocol-buffers/docs/csharptutorial)
* [ProtoBuf v3.12.3 Down 🌍](https://github.com/protocolbuffers/protobuf/releases/tag/v3.12.3)

* Protobuf자체를 빌드할게 아니라면 **protoc-3.12.3-win64.zip**을 다운받으면 된다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity-adv/unity-adv-9-1.png"/>
</p>

* Common아래 Protoc를 넣자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity-adv/unity-adv-9-2.png"/>
</p>

* ProtoBuf의 사용을 위해선 NuGet의 설치가 필요하다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity-adv/unity-adv-9-3.png"/>
</p>

```bat
:: GenProto.bat

protoc.exe -I=./ --csharp_out=./ ./Protocol.proto 
IF ERRORLEVEL 1 PAUSE

START ../../../Server/PacketGenerator/bin/PacketGenerator.exe ./Protocol.proto
XCOPY /Y Protocol.cs "../../../Client/Assets/Scripts/Packet"
XCOPY /Y Protocol.cs "../../../Server/Server/Packet"
XCOPY /Y ClientPacketManager.cs "../../../Client/Assets/Scripts/Packet"
XCOPY /Y ServerPacketManager.cs "../../../Server/Server/Packet"
```

```csharp
namespace Server
{
	class ClientSession : PacketSession
	{
		public int SessionId { get; set; }

		public override void OnConnected(EndPoint endPoint)
		{
			Console.WriteLine($"OnConnected : {endPoint}");

			// PROTO Test
			S_Chat chat = new S_Chat()
			{
				Context = "안녕하세요"
			};

			ushort size = (ushort)chat.CalculateSize();
			byte[] sendBuffer = new byte[size + 4];
			Array.Copy(BitConverter.GetBytes(size + 4), 0, sendBuffer, 0, sizeof(ushort));
			ushort protocolId = (ushort)MsgId.SChat;
			Array.Copy(BitConverter.GetBytes(protocolId), 0, sendBuffer, 2, sizeof(ushort));
			Array.Copy(chat.ToByteArray(), 0, sendBuffer, 4, size);

			Send(new ArraySegment<byte>(sendBuffer));

            // ...
```