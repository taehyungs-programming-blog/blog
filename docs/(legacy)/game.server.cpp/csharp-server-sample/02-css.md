---
layout: default
title: "02. 효과적으로 DB READ/WRITE하기"
parent: "(C# Server Sample)"
grand_parent: "Game Server 👾"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/kthCodeLog/csharp-gameserver-sample/tree/02)

* 추가 처리될 부분
    * SaveChange는 실패할 수 있는데 예외처리가 필요
    * DB는 READ/WRITE 속도가 느리기에 이부분의 처리가 필요

---

## SaveChange는 실패할 수 있는데 예외처리가 필요

```csharp
// 사실 별거없다 ㅎ

namespace Server
{
	public static class Extensions
	{
		public static bool SaveChangesEx(this AppDbContext db)
		{
			try
			{
				db.SaveChanges();
				return true;
			}
			catch
			{
				return false;
			}
		}
	}
}
```

---

## DB는 READ/WRITE 속도가 느리기에 이부분의 처리가 필요

```csharp
// DbTransaction를 만들어 Job으로 처리한다.

public class DbTransaction : JobSerializer
{
    public static DbTransaction Instance { get; } = new DbTransaction();

    // Me (GameRoom) -> You (Db) -> Me (GameRoom)
    public static void SavePlayerStatus_AllInOne(Player player, GameRoom room)
    {
        if (player == null || room == null)
            return;

        // Me (GameRoom)
        PlayerDb playerDb = new PlayerDb();
        playerDb.PlayerDbId = player.PlayerDbId;
        playerDb.Hp = player.Stat.Hp;

        // You
        Instance.Push(() =>
        {
            using (AppDbContext db = new AppDbContext())
            {
                db.Entry(playerDb).State = EntityState.Unchanged;
                db.Entry(playerDb).Property(nameof(PlayerDb.Hp)).IsModified = true;
                bool success = db.SaveChangesEx();
                if (success)
                {
                    // Me
                    room.Push(() => Console.WriteLine($"Hp Saved({playerDb.Hp})"));
                }
            }
        });			
    }

    // Me (GameRoom)
    public static void SavePlayerStatus_Step1(Player player, GameRoom room)
    {
        if (player == null || room == null)
            return;

        // Me (GameRoom)
        PlayerDb playerDb = new PlayerDb();
        playerDb.PlayerDbId = player.PlayerDbId;
        playerDb.Hp = player.Stat.Hp;
        Instance.Push<PlayerDb, GameRoom>(SavePlayerStatus_Step2, playerDb, room);
    }

    // ...
```

```csharp
// Command Pattern이기에 주기적으로 처리할 thread가 필요하다

static void Main(string[] args)
{
    // ...

    // TODO
    while (true)
    {
        // Main에서 주기적 Flush
        DbTransaction.Instance.Flush();
    }
```
