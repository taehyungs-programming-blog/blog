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

🛵 데이터엔 두 가지 데이터가 존재한다.<br>
🛵 **Principal Entity** - 일반적 데이터 (Player)<br>
🛵 **Dependent Entity** - 의존적 데이터, Foreign Key를 포함하는 쪽 (Item)<br>
🛵 그렇다면 Dependent Entity는 Principal Entity없이 존재가 가능할까?(Null을 넣을수 있나?)<br>
🛵 예를 들자면 주인이 없는 Item이 존재할수 있는데 그럼 주인(Owner)에 Null을 넣을수 있냐는 문제이다.<br>

```csharp
// 결론부터 말하면 nullable 변수를 선언하면 된다.
int? nullablevar;
```

```csharp
public class Item
{
    public int ItemId { get; set; }
    public int TemplateId { get; set; }
    public DateTime CreatedDate { get; set; }

    // 아직은 Nullable이 아니기에
    public int OwnerId { get; set; }
    public Player Owner { get; set; }   
```

```csharp
public static void ShowItems()
{
    using (AppDbContext db = new AppDbContext())
    {
        foreach (var item in db.Items.Include(i => i.Owner).ToList())
        {
            if (item.Owner == null)
                Console.WriteLine($"ItemId({item.ItemId}) TemplateId({item.TemplateId}) Owner(0)");
            else
                Console.WriteLine($"ItemId({item.ItemId}) TemplateId({item.TemplateId}) Owner({item.Owner.PlayerId}) Owner({item.Owner.Name})");
        }
    }
}

public static void Test()
{
    ShowItems();

    Console.WriteLine("Input Delete PlayerId");
    Console.Write("> ");
    int id = int.Parse(Console.ReadLine());

    using (AppDbContext db = new AppDbContext())
    {
        Player player = db.Players
            .Include(p => p.Item)
            .Single(p => p.PlayerId == id);

        db.Players.Remove(player);
        db.SaveChanges();
    }

    Console.WriteLine("--- Test Complete ---");
    ShowItems();
}
```

🛵 Player가 지워지면 FK로 해당 Player를 참조하는 Item도 같이 삭제된다.

```
명령어 입력
[0] Force Reset
[1] Test
> 1
ItemId(1) TemplateId(101) Owner(1) Owner(Taehyung)
ItemId(2) TemplateId(102) Owner(2) Owner(Unkwon Player 1)
ItemId(3) TemplateId(103) Owner(3) Owner(Unkwon Player 2)
Input Delete PlayerId
> 1
--- Test Complete ---
ItemId(2) TemplateId(102) Owner(2) Owner(Unkwon Player 1)
ItemId(3) TemplateId(103) Owner(3) Owner(Unkwon Player 2)
>
```

```csharp
public class Item
{
    public int ItemId { get; set; }
    public int TemplateId { get; set; }
    public DateTime CreatedDate { get; set; }

    // Nullable로 만들어 보면
    public int? OwnerId { get; set; }
    public Player Owner { get; set; }   
```

```
명령어 입력
[0] Force Reset
[1] Test
> 0
!!! DB is Reset !!!
> 1
ItemId(1) TemplateId(101) Owner(1) Owner(Taehyung)
ItemId(2) TemplateId(102) Owner(2) Owner(Unkwon Player 1)
ItemId(3) TemplateId(103) Owner(3) Owner(Unkwon Player 2)
Input Delete PlayerId
> 1
--- Test Complete ---
ItemId(1) TemplateId(101) Owner(0)
ItemId(2) TemplateId(102) Owner(2) Owner(Unkwon Player 1)
ItemId(3) TemplateId(103) Owner(3) Owner(Unkwon Player 2)
>
```

---

## Relationship Update

* [Get Code 🌍](https://github.com/EasyCoding-7/MMO_EFCore_Tutorial/tree/4-5)

```csharp
// 1:1 Relationship Update

public static void Update_1v1()
{
    ShowItems();

    Console.WriteLine("Input ItemSwitch PlayerId");
    Console.Write("> ");
    int id = int.Parse(Console.ReadLine());

    using (AppDbContext db = new AppDbContext())
    {
        Player player = db.Players
            .Include(p => p.Item)
            .Single(p => p.PlayerId == id);

        player.Item = new Item()
        {
            TemplateId = 777,
            CreatedDate = DateTime.Now
        };

        db.SaveChanges();
    }

    Console.WriteLine("--- Test Complete ---");
    ShowItems();
}
```

```
명령어 입력
[0] Force Reset
[1] Update 1v1
> 0
!!! DB is Reset !!!
> 1
ItemId(1) TemplateId(101) Owner(1) Owner(Taehyung)
ItemId(2) TemplateId(102) Owner(2) Owner(Unkwon Player 1)
ItemId(3) TemplateId(103) Owner(3) Owner(Unkwon Player 2)
Input ItemSwitch PlayerId
> 1
--- Test Complete ---
ItemId(1) TemplateId(101) Owner(0)
ItemId(2) TemplateId(102) Owner(2) Owner(Unkwon Player 1)
ItemId(3) TemplateId(103) Owner(3) Owner(Unkwon Player 2)
ItemId(4) TemplateId(777) Owner(1) Owner(Taehyung)
>
```

```csharp
public static void Update_1vN()
{
    ShowGuild();

    Console.WriteLine("Input GuildId");
    Console.Write("> ");
    int id = int.Parse(Console.ReadLine());

    using (AppDbContext db = new AppDbContext())
    {
        Guild guild = db.Guilds
            .Include(p => p.Members)
            .Single(p => p.GuildId == id);

        guild.Members.Add(new Player()
        {
            Name = "NeBee"
        });

        db.SaveChanges();
    }

    Console.WriteLine("--- Test Complete ---");
    ShowGuild();
}
```

---

## Delete

* [Get Code 🌍](https://github.com/EasyCoding-7/MMO_EFCore_Tutorial/tree/4-6)

```csharp
public static void TestDelete()
{
    ShowItems();

    Console.WriteLine("Select Delete ItemId");
    Console.Write("> ");
    int id = int.Parse(Console.ReadLine());

    using (AppDbContext db = new AppDbContext())
    {
        Item item = db.Items.Find(id);
        db.Items.Remove(item); 
        db.SaveChanges();
    }

    Console.WriteLine("--- TestDelete Complete ---");
    ShowItems();
}
```

🛵 만약 삭제했던 아이템을 복구해야한다면? <br>
🛵 약간의 트릭이 필요하다

```csharp
[Table("Item")]
public class Item
{
    // softdelete를 만들어둔다.
    public bool SoftDeleted { get; set; }

    // ...
```

```csharp
    public class AppDbContext : DbContext
    {
        // ...

        protected override void OnModelCreating(ModelBuilder builder)
        {
            // Item에 접근시 필터링
            builder.Entity<Item>().HasQueryFilter(i => i.SoftDeleted == false);
        }
```

```csharp
public static void TestDelete()
{
    ShowItems();

    Console.WriteLine("Select Delete ItemId");
    Console.Write("> ");
    int id = int.Parse(Console.ReadLine());

    using (AppDbContext db = new AppDbContext())
    {
        Item item = db.Items.Find(id);
        // db.Items.Remove(item); 
        item.SoftDeleted = true;    
        db.SaveChanges();
    }

    Console.WriteLine("--- TestDelete Complete ---");
    ShowItems();
}
```