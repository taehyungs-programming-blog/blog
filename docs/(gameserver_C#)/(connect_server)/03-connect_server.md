---
layout: default
title: "03. Move"
parent: "(컨텐츠 제작 1 - 서버 연결)"
grand_parent: "(GameServer C# 🎯)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 📥](https://github.com/Arthur880708/CSharp_GameServer_Connect_To_Server/tree/12)

## 움직임과 관련된 패킷을 추가해 보자.

```json
syntax = "proto3";

package Protocol;
import "google/protobuf/timestamp.proto";
option csharp_namespace = "Google.Protobuf.Protocol";

enum MsgId {
  S_ENTER_GAME = 0;
  S_LEAVE_GAME = 1;
  S_SPAWN = 2;
  S_DESPAWN = 3;
  C_MOVE = 4;
  S_MOVE = 5;
}

enum CreatureState {
  IDLE = 0;
  MOVING = 1;
  SKILL = 2;
  DEAD = 3;
}

enum MoveDir {
  NONE = 0;
  UP = 1;
  DOWN = 2;
  LEFT = 3;
  RIGHT = 4;
}

// ...

message S_Move {
  int32 playerId = 1;
  PositionInfo posInfo = 2;
}

message PlayerInfo {
  int32 playerId = 1;
  string name = 2;
  PositionInfo posInfo = 3;
}

message PositionInfo {
  CreatureState state = 1;
  MoveDir moveDir = 2;
  int32 posX = 3;
  int32 posY = 4;
}
```

---

## MoveHandler 구현

```csharp
class PacketHandler
{
	public static void C_MoveHandler(PacketSession session, IMessage packet)
	{
		C_Move movePacket = packet as C_Move;
		ClientSession clientSession = session as ClientSession;

		Console.WriteLine($"C_Move ({movePacket.PosInfo.PosX}, {movePacket.PosInfo.PosY})");

		if (clientSession.MyPlayer == null)
			return;
		if (clientSession.MyPlayer.Room == null)
			return;

		// TODO : 검증

		// 일단 서버에서 좌표 이동
		PlayerInfo info = clientSession.MyPlayer.Info;
		info.PosInfo = movePacket.PosInfo;

		// 다른 플레이어한테도 알려준다
		S_Move resMovePacket = new S_Move();
		resMovePacket.PlayerId = clientSession.MyPlayer.Info.PlayerId;
		resMovePacket.PosInfo = movePacket.PosInfo;

		clientSession.MyPlayer.Room.Broadcast(resMovePacket);
	}
}
```