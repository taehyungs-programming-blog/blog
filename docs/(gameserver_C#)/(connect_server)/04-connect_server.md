---
layout: default
title: "04. Skill"
parent: "(컨텐츠 제작 1 - 서버 연결)"
grand_parent: "(GameServer C# 🎯)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 📥](https://github.com/Arthur880708/CSharp_GameServer_Connect_To_Server/tree/16)

## 스킬 & 히트

```json
// ...

message C_Skill {
  SkillInfo info = 1;
}

message S_Skill {
  int32 playerId = 1;
  SkillInfo info = 2;
}

// ...

message SkillInfo {
  int32 skillId = 1;
}
```

```csharp
public static void C_SkillHandler(PacketSession session, IMessage packet)
{
    C_Skill skillPacket = packet as C_Skill;
    ClientSession clientSession = session as ClientSession;

    Player player = clientSession.MyPlayer;
    if (player == null)
        return;

    GameRoom room = player.Room;
    if (room == null)
        return;

    room.HandleSkill(player, skillPacket);
}
```

```csharp
public void HandleSkill(Player player, C_Skill skillPacket)
{
    if (player == null)
        return;

    lock (_lock)
    {
        PlayerInfo info = player.Info;
        if (info.PosInfo.State != CreatureState.Idle)
            return;

        // TODO : 스킬 사용 가능 여부 체크

        // 통과
        info.PosInfo.State = CreatureState.Skill;

        S_Skill skill = new S_Skill() { Info = new SkillInfo() };
        skill.PlayerId = info.PlayerId;
        skill.Info.SkillId = 1;
        Broadcast(skill);

        // TODO : 데미지 판정
        Vector2Int skillPos = player.GetFrontCellPos(info.PosInfo.MoveDir);
        Player target = _map.Find(skillPos);
        if (target != null)
        {
            Console.WriteLine("Hit Player !");
        }
    }
}
```

---

## 화살

```csharp
public void HandleSkill(Player player, C_Skill skillPacket)
{
    // ...

        else if (skillPacket.Info.SkillId == 2)
        {
            Arrow arrow = ObjectManager.Instance.Add<Arrow>();
            if (arrow == null)
                return;

            arrow.Owner = player;
            arrow.PosInfo.State = CreatureState.Moving;
            arrow.PosInfo.MoveDir = player.PosInfo.MoveDir;
            arrow.PosInfo.PosX = player.PosInfo.PosX;
            arrow.PosInfo.PosY = player.PosInfo.PosY;
            EnterGame(arrow);
        }
    }
}
```

```csharp
public void EnterGame(GameObject gameObject)
{
    // ...

        // Projectile - player와 monster를 제외한 object을 의미
        else if (type == GameObjectType.Projectile)
        {
            Projectile projectile = gameObject as Projectile;
            _projectiles.Add(gameObject.Id, projectile);
            projectile.Room = this;
        }
    
        // 타인한테 정보 전송
        {
            S_Spawn spawnPacket = new S_Spawn();
            spawnPacket.Objects.Add(gameObject.Info);
            foreach (Player p in _players.Values)
            {
                if (p.Id != gameObject.Id)
                    p.Session.Send(spawnPacket);
            }
        }
    }
}
```

```csharp
namespace Server.Game
{
	public class Arrow : Projectile
	{
		public GameObject Owner { get; set; }

		long _nextMoveTick = 0;

		public override void Update()
		{
			if (Owner == null || Room == null)
				return;

			if (_nextMoveTick >= Environment.TickCount64)
				return;

			_nextMoveTick = Environment.TickCount64 + 50;

			Vector2Int destPos = GetFrontCellPos();
			if (Room.Map.CanGo(destPos))
			{
				CellPos = destPos;

				S_Move movePacket = new S_Move();
				movePacket.ObjectId = Id;
				movePacket.PosInfo = PosInfo;
				Room.Broadcast(movePacket);

				Console.WriteLine("Move Arrow");
			}
			else
			{
				GameObject target = Room.Map.Find(destPos);
				if (target != null)
				{
					// TODO : 피격 판정
				}

				// 소멸
				Room.LeaveGame(Id);
			}
		}
	}
}

```