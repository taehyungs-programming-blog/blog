---
layout: default
title: "01. ProtoBuf"
parent: "(컨텐츠 제작 1 - 서버 연결)"
grand_parent: "(GameServer C# 🎯)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 준비

* [Protocol Buffers v3.12.3](https://github.com/protocolbuffers/protobuf/releases/tag/v3.12.3) 에서 **protoc-3.12.3-win64.zip** 다운
* 사용할 폴더에 넣어둔다

* 솔루션 우클릭 -> 솔루션용 NuGet 패키지 관리 -> Google.Protobuf 다운

---

## .proto 파일 생성

```json
syntax = "proto3";

package Protocol;
import "google/protobuf/timestamp.proto";
option csharp_namespace = "Google.Protobuf.Protocol";

enum MsgId {
  PERSON = 0;
  C_CHAT = 1;
  S_CHAT = 2;
  S_ENTER_GAME = 3;
}

message C_Chat {
  string context = 1;
}

message S_Chat {
  string context = 1;
}

message S_EnterGame {
  string context = 1;
}

message Person {
  int32 packetHeader = 1;
  string name = 2;
  int32 id = 3;  // Unique ID number for this person.
  string email = 4;

  enum PhoneType {
    MOBILE = 0;
    HOME = 1;
    WORK = 2;
  }

  message PhoneNumber {
    string number = 1;
    PhoneType type = 2;
  }

  repeated PhoneNumber phones = 5;
  google.protobuf.Timestamp last_updated = 6;
}

// Our address book file is just one of these.
message AddressBook {
  repeated Person people = 1;
}
```

```bat
protoc.exe -I=./ --csharp_out=./ ./Protocol.proto 
IF ERRORLEVEL 1 PAUSE

START ../../../Server/PacketGenerator/bin/PacketGenerator.exe ./Protocol.proto
XCOPY /Y Protocol.cs "../../../Client/Assets/Scripts/Packet"
XCOPY /Y Protocol.cs "../../../Server/Server/Packet"
XCOPY /Y ClientPacketManager.cs "../../../Client/Assets/Scripts/Packet"
XCOPY /Y ServerPacketManager.cs "../../../Server/Server/Packet"
```

* 사용은 이렇게 한다. (송신)

```csharp
public override void OnConnected(EndPoint endPoint)
{
    Console.WriteLine($"OnConnected : {endPoint}");

    // PROTO buf 객체 생성
    S_Chat chat = new S_Chat()
    {
        Context = "안녕하세요"
    };

    ushort size = (ushort)chat.CalculateSize();
    byte[] sendBuffer = new byte[size + 4]; 
    // 4바이트를 더 하는이유는 size, id 정보를 포함하기 위함(아래참고)

    // 첫 2바이트에 size 정보
    Array.Copy(BitConverter.GetBytes(size + 4), 0, sendBuffer, 0, sizeof(ushort));

    // 다음 2바이트에 id 정보
    ushort protocolId = (ushort)MsgId.SChat;
    Array.Copy(BitConverter.GetBytes(protocolId), 0, sendBuffer, 2, sizeof(ushort));

    // 나머지는 실제 데이터
    Array.Copy(chat.ToByteArray(), 0, sendBuffer, 4, size);

    Send(new ArraySegment<byte>(sendBuffer));
```

* 수신

```csharp
public void OnRecvPacket(PacketSession session, ArraySegment<byte> buffer)
{
    ushort count = 0;

    ushort size = BitConverter.ToUInt16(buffer.Array, buffer.Offset);
    count += 2;
    ushort id = BitConverter.ToUInt16(buffer.Array, buffer.Offset + count);
    count += 2;

    Action<PacketSession, ArraySegment<byte>, ushort> action = null;
    if (_onRecv.TryGetValue(id, out action))
        action.Invoke(session, buffer, id);
}
```

---

* [Get Code 🌍](https://github.com/Arthur880708/CSharp_GameServer_Connect_To_Server/tree/8)

## 사전 개선사항

* Message Send하는 부분을 함수로 묶어보자.

```csharp
class ClientSession : PacketSession
{
    public int SessionId { get; set; }

/*
  public enum MsgId {
    [pbr::OriginalName("PERSON")] Person = 0,
    [pbr::OriginalName("C_CHAT")] CChat = 1,
    [pbr::OriginalName("S_CHAT")] SChat = 2,
    [pbr::OriginalName("S_ENTER_GAME")] SEnterGame = 3,
  }
*/

    public void Send(IMessage packet)
    {
        string msgName = packet.Descriptor.Name.Replace("_", string.Empty);
        
        // C# 기능 Enum으로 부터 id값 받기
        MsgId msgId = (MsgId)Enum.Parse(typeof(MsgId), msgName);

        // Size
        ushort size = (ushort)packet.CalculateSize();

        // Buffer 할당
        byte[] sendBuffer = new byte[size + 4];

        // 2바이트 사이즈
        Array.Copy(BitConverter.GetBytes(size + 4), 0, sendBuffer, 0, sizeof(ushort));

        // 2바이트 msg id
        Array.Copy(BitConverter.GetBytes((ushort)msgId), 0, sendBuffer, 2, sizeof(ushort));

        // 나머진 데이터
        Array.Copy(packet.ToByteArray(), 0, sendBuffer, 4, size);

        // Send!!!
        Send(new ArraySegment<byte>(sendBuffer));
    }
```

```csharp
// 실사용은 이렇게 한다.
public override void OnConnected(EndPoint endPoint)
{
    Console.WriteLine($"OnConnected : {endPoint}");

    // PROTO Test
    S_Chat chat = new S_Chat()
    {
        Context = "안녕하세요"
    };

    Send(chat);
}
```