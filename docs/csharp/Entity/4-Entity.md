---
layout: default
title: "4. 데이터 수정(Create, Update, Delete)"
parent: (Entity)
grand_parent: C#
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 상태관리

* [Get Code 🌍](https://github.com/EasyCoding-7/MMO_EFCore_Tutorial/tree/4-1)

🛵 당연한 얘기겠지만 db의 데이터 업데이트를 위해 데이터의 상태관리가 필요하다<br>
🛵 그 상태관리를 어떻게 할 것인가에 관한 설명!

```csharp
        public static void CreateTestData(AppDbContext db)
        {
            var player_kth = new Player() { Name = "Taehyung" };
            var player1 = new Player() { Name = "Unkwon Player 1" };
            var player2 = new Player() { Name = "Unkwon Player 2" };

            // 이런식으로 state 확인가능
            EntityState state = db.Entry(player_kth).State;
```

```csharp
// State에는 네 가지 State가 존재
public enum EntityState
{
    Detached,   // No Tracking - 추적이 되지 않는 상태, SaveChanges를 해도 안됨
    Unchanged,  // DB에는 데이터가 있으나 수정사항도 없음. SaveChanges를 해도 변화 없음.
    Deleted,    // DB에는 데이터가 있으나, DB를 삭제할 예정 SaveChanges후 삭제된다.
    Modified,   // DB에는 데이터가 있으나, 변경될 예정
    Added       // DB에는 데이터가 없으나, 추가될 예정
}
```

🛵 Example

```csharp
public static void CreateTestData(AppDbContext db)
{
    var player_kth = new Player()
    {
        Name = "Taehyung"
    };
    var player1 = new Player()
    {
        Name = "Unkwon Player 1"
    };
    var player2 = new Player()
    {
        Name = "Unkwon Player 2"
    };

    Console.WriteLine(db.Entry(player_kth).State);  // Detached

    List<Item> items = new List<Item>()
    {
        // ...
    };

    Guild guild = new Guild()
    {
        GuildName = "g",
        Members = new List<Player>() { player1, player2 }
    };

    db.Items.AddRange(items);
    db.Guilds.Add(guild);

    Console.WriteLine(db.Entry(player_kth).State);  // Added

    db.SaveChanges();

    Console.WriteLine(db.Entry(player_kth).State);  // Unchanged
}
```

---

## Update

* [Get Code 🌍](https://github.com/EasyCoding-7/MMO_EFCore_Tutorial/tree/4-2)

```csharp
public static void UpdateTest()
{
    /*
    * Update의 단계
    * 1. Tracked Entity를 얻는다
    * 2. Entity 클래스의 property를 변경한다.(set)
    * 3. SaveChangeds를 호출한다.
    */

    using (AppDbContext db = new AppDbContext())
    {
        // 참고로 Single은 데이터가 한 개 있을꺼라는 가정(틀릴시 Exception)
        var guild = db.Guilds.Single(g => g.GuildName == "g");

        guild.GuildName = "DNG";
        db.SaveChanges();
    }
}
```

---

## Connected, DisConnected Update

* [Get Code 🌍](https://github.com/EasyCoding-7/MMO_EFCore_Tutorial/tree/4-3)

🛵 **Connected** - 업데이트의 3단계가 쭉 일어남.<br>
🛵 **DisConnected** - 업데이트의 3단계가 쭉 일어나지 않음(중간에 끊어질 수 있음)<br>
🛵 업데이트 방식이 두 가지로 나뉘게 되는데 **Reload**, **Full Update**<br>
🛵 **Reload** - 데이터 수신시 마다 업데이트<br>
🛵 **Full Update** - 필요한 데이터를 모두 수신 후 통으로 업데이트<br>

```csharp
public static void ShowGuilds()
{
    using (AppDbContext db = new AppDbContext())
    {
        foreach (var guild in db.Guilds.MapGuildToDto())
        {
            Console.WriteLine($"GuildId({guild.GuildId}) GuildName({guild.Name}) MemberCount({guild.MemberCount})");
        }
    }
}

public static void UpdateReload()
{
    /*
        * 장점 : 최소한의 데이터로 업데이트 가능
        * 단점 : Read를 두 번 해야한다.(큰 단점은 아님)
        */

    ShowGuilds();

    Console.WriteLine("Input Guild-Id");
    Console.Write("> ");
    int id = int.Parse(Console.ReadLine());

    Console.WriteLine("Input Guild-Name");
    Console.Write("> ");
    string name = Console.ReadLine();

    using (AppDbContext db = new AppDbContext())
    {
        Guild guild = db.Find<Guild>(id);
        guild.GuildName = name;
        db.SaveChanges();
    }

    Console.WriteLine("--- Update Complete ---");
    ShowGuilds();
}

public static string MakeUpdateJsonStr()
{
    var jsonStr = "{\"GuildId\":1, \"GuildName\":\"Hello\", \"Members\":null}";
    return jsonStr;
}
public static void UpdateFull()
{
    /*
        * 장점 : Read를 한 번만 해야함
        * 단점 : 모든 데이터를 알아야 Update가능(보안이슈)
        */

    ShowGuilds();

    string jsonStr = MakeUpdateJsonStr();
    Guild guild = JsonConvert.DeserializeObject<Guild>(jsonStr);

    using (AppDbContext db = new AppDbContext())
    {
        db.Guilds.Update(guild);
        db.SaveChanges();
    }

    Console.WriteLine("--- Update Complete ---");
    ShowGuilds();
}
```

---

## Foreign Key와 Nullable

* [Get Code 🌍](https://github.com/EasyCoding-7/MMO_EFCore_Tutorial/tree/4-4)


