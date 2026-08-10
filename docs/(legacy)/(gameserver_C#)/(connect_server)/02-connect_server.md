---
layout: default
title: "02. 게임 입장"
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

* [Get Code 📥](https://github.com/Arthur880708/CSharp_GameServer_Connect_To_Server/tree/10)

* 메시지를 좀 수정해 보자.
    * Enter, Leave, Move, Info 추가

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

message S_EnterGame {
  PlayerInfo player = 1;
}

message S_LeaveGame {
}

message S_Spawn {
  repeated PlayerInfo players = 1;
}

message S_Despawn {
  repeated int32 playerIds = 1;
}

message C_Move {
  int32 posX = 1;
  int32 posY = 2;
}

message S_Move {
  int32 playerId =1;
  int32 posX = 2;
  int32 posY = 3;
}

message PlayerInfo {
  int32 playerId = 1;
  string name = 2;
  int32 posX = 3;
  int32 posY = 4;
}
```

---

## 수신측 복습!

```csharp
class PacketManager
{
	// ...

	PacketManager()
	{
		Register();
	}

	// ...
	public void Register()
	{		
        // PacketManager 생성과 동시에 등록
		_onRecv.Add((ushort)MsgId.CMove, MakePacket<C_Move>);
		_handler.Add((ushort)MsgId.CMove, PacketHandler.C_MoveHandler);
	}

    Dictionary<ushort, Action<PacketSession, ArraySegment<byte>, ushort>> _onRecv = 
    new Dictionary<ushort, Action<PacketSession, ArraySegment<byte>, ushort>>();

	Dictionary<ushort, Action<PacketSession, IMessage>> _handler = 
    new Dictionary<ushort, Action<PacketSession, IMessage>>();

    // ...
```

```csharp
void MakePacket<T>(PacketSession session, ArraySegment<byte> buffer, ushort id) where T : IMessage, new()
{
    T pkt = new T();
    pkt.MergeFrom(buffer.Array, buffer.Offset + 4, buffer.Count - 4);
    Action<PacketSession, IMessage> action = null;
    if (_handler.TryGetValue(id, out action))
        action.Invoke(session, pkt);
}
```

```csharp
class PacketHandler
{
	public static void C_MoveHandler(PacketSession session, IMessage packet)
	{
		C_Move movePacket = packet as C_Move;
		ClientSession serverSession = session as ClientSession;

	}
}
```

---

## Class 추가

* GameRoom.cs - 서버를 의미
* Player.cs - 서내 접속하는 캐릭터 의미
* PlayerManager.cs - 서버 PC에 접속한 캐릭터 의미
* RoomManager.cs - 서버를 관리하는 통합서버? (월드)를 의미

```csharp
public override void OnConnected(EndPoint endPoint)
{
    Console.WriteLine($"OnConnected : {endPoint}");

    // 캐릭터 접속시 생성된 Player를 Add
    MyPlayer = PlayerManager.Instance.Add();
    {
        MyPlayer.Info.Name = $"Player_{MyPlayer.Info.PlayerId}";
        MyPlayer.Info.PosX = 0;
        MyPlayer.Info.PosY = 0;
        MyPlayer.Session = this;
    }

    // 오늘의 핵심은 GameRoom인데
        // 아래서 Find(1)을 한 이유는 현재 서버는 1개라서
    RoomManager.Instance.Find(1).EnterGame(MyPlayer);
}
```

```csharp
public class PlayerManager
{
    public static PlayerManager Instance { get; } = new PlayerManager();

    object _lock = new object();
    Dictionary<int, Player> _players = new Dictionary<int, Player>();
    int _playerId = 1; // TODO
    
    public Player Add()
    {
        Player player = new Player();

        lock (_lock)
        {
            player.Info.PlayerId = _playerId;
            _players.Add(_playerId, player);
            _playerId++;
        }

        return player;
    }
```

```csharp
namespace Server.Game
{
	public class GameRoom
	{
		object _lock = new object();
		public int RoomId { get; set; }

		List<Player> _players = new List<Player>();

		public void EnterGame(Player newPlayer)
		{
			if (newPlayer == null)
				return;

			lock (_lock)
			{
				_players.Add(newPlayer);
				newPlayer.Room = this;

				// 본인한테 정보 전송
				{
					S_EnterGame enterPacket = new S_EnterGame();
					enterPacket.Player = newPlayer.Info;
					newPlayer.Session.Send(enterPacket);

					S_Spawn spawnPacket = new S_Spawn();
					foreach (Player p in _players)
					{
						if (newPlayer != p)
							spawnPacket.Players.Add(p.Info);
					}
					newPlayer.Session.Send(spawnPacket);
				}

				// 타인한테 정보 전송
				{
					S_Spawn spawnPacket = new S_Spawn();
					spawnPacket.Players.Add(newPlayer.Info);
					foreach (Player p in _players)
					{
						if (newPlayer != p)
							p.Session.Send(spawnPacket);
					}
				}
			}
		}

		public void LeaveGame(int playerId)
		{
			lock (_lock)
			{
				Player player = _players.Find(p => p.Info.PlayerId == playerId);
				if (player == null)
					return;

				_players.Remove(player);
				player.Room = null;

				// 본인한테 정보 전송
				{
					S_LeaveGame leavePacket = new S_LeaveGame();
					player.Session.Send(leavePacket);
				}

				// 타인한테 정보 전송
				{
					S_Despawn despawnPacket = new S_Despawn();
					despawnPacket.PlayerIds.Add(player.Info.PlayerId);
					foreach (Player p in _players)
					{
						if (player != p)
							p.Session.Send(despawnPacket);
					}
				}
			}
		}
	}
}
```