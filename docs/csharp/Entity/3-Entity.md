---
layout: default
title: "3. Read (데이터 쿼리)"
parent: (Entity)
grand_parent: C#
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Relationship 이란?

* [Get Code 🌍](https://github.com/EasyCoding-7/MMO_EFCore_Tutorial/tree/3-1)

👾 세 가지 Relationship이 있을 수 있다.<br>
👾 **1:1**, **1:N**, **N:N**<Br>
👾 현재 Player와 Item은 1:N의 관계라 할 수 있음.<br>
👾 Item에선 Player를 Foreign Key로 관리중이나 Player는 Item을 별도로 관리중이 아니니 관리하게 만들어 보자면

```csharp
public class Item
{
    public int ItemId { get; set; }
    public int TemplateId { get; set; }
    public DateTime CreatedDate { get; set; }

    public Player Owner { get; set; }   
    public int OwnerId { get; set; }
}

public class Player
{
    public int PlayerId { get; set; }
    public string Name { get; set; }

    // Items를 Foreign Key로 넣는다.
        // 이러면 끝? PlayerId를 기반으로 entity가 알아서 해준다(좋그만)
    public ICollection<Item> Items { get; set; }
}
```

👾 Player의 Items를 읽어보자.

```csharp
public static void ShowItems()
{
    ReadAll();

    Console.WriteLine("플레이어 이름 : ");
    Console.WriteLine("> ");
    string name = Console.ReadLine();

    using (var db = new AppDbContext())
    {
        foreach (Player player in db.Players.AsNoTracking().Where(p => p.Name == name).Include(p=>p.Items))
        {
            foreach(Item item in player.Items)
            {
                Console.WriteLine($"Items {item.TemplateId}");
            }
        }
    }
}
```

```csharp
namespace MMO_EFCore_Tutorial
{
    // 테이블 이름 Item으로 고정(안할시 변수명으로 설정됨)
    [Table("Item")]
    public class Item
    {
        // PK(Primary Key)
        public int ItemId { get; set; }
        public int TemplateId { get; set; }
        public DateTime CreatedDate { get; set; }

        
        public Player Owner { get; set; }   
        // OwnerId를 지울 경우 Foreign Key로서의 기능을 잃게된다.
        //public int OwnerId { get; set; }
        // [ForeignKey=("OwnerId")] 이런식으로 직접 입력도 가능
    }
```

---

## READ를 하는 세 가지 방법

* [Get Code 🌍](https://github.com/EasyCoding-7/MMO_EFCore_Tutorial/tree/3-2)

* `EagerLoading`
* `ExplicitLoading`
* `SelectLoading`

```csharp
// 테이블의 구조를 조금 수정함.

namespace MMO_EFCore_Tutorial
{
    // 테이블 이름 Item으로 고정(안할시 변수명으로 설정됨)
    [Table("Item")]
    public class Item
    {
        // PK(Primary Key)
        public int ItemId { get; set; }
        public int TemplateId { get; set; }
        public DateTime CreatedDate { get; set; }

        [ForeignKey("OwnerId")] 
        public Player Owner { get; set; }   
    }

    [Table("Player")]
    public class Player
    {
        public int PlayerId { get; set; }
        public string Name { get; set; }

        // Player와 Item은 1:1의 관계이고
        public Item Item { get; set; }

        // Player와 Guild는 1:N의 관계이다.
        public Guild Guild { get; set; }
    }

    [Table("Guild")]
    public class Guild
    {
        public int GuildId { get; set;}
        public string GuildName { get; set; }

        public ICollection<Player> Members { get; set; }
    }
}

```

```csharp
public static void EagerLoading()
{
    Console.WriteLine("길드 이름은? :");
    Console.Write("> ");
    string name = Console.ReadLine();

    // 장점 : DB 접근을 한 번에
    // 단점 : 안쓰는 DB까지 로드할 수 있음

    using (var db = new AppDbContext())
    {
        Guild guild = db.Guilds.AsNoTracking()
            .Where(g => g.GuildName == name)
            .Include(g => g.Members)    // Foreign인 Player 조회 예정
            .ThenInclude(p => p.Item)   // Foreign인 Item까지 조회 예정
            .First();                   // 하나만 찾아주세요

        foreach(Player player in guild.Members)
        {
            Console.WriteLine($"ItemId - {player.Item.TemplateId} / Owner - {player.Name}");
        }
    }
}
```

```csharp
public static void ExplicitLoading()
{
    Console.WriteLine("길드 이름은? :");
    Console.Write("> ");
    string name = Console.ReadLine();

    // 장점 : 필요한 DB만 특정시점에 로드
    // 단점 : 불필요한 선회가 있을 수 있음

    using (var db = new AppDbContext())
    {
        Guild guild = db.Guilds
            .Where(g => g.GuildName == name)
            .First();                   // 하나만 찾아주세요

        // Explicit
        db.Entry(guild).Collection(g=>g.Members).Load();        // Player를 로드해주세요
        
        foreach(Player player in guild.Members)
        {
            db.Entry(player).Reference(p => p.Item).Load();
        }

        foreach (Player player in guild.Members)
        {
            Console.WriteLine($"ItemId - {player.Item.TemplateId} / Owner - {player.Name}");
        }
    }
}
```

```csharp
public static void SelectLoading()
{
    // 길드원의 수만 출력하고 싶다면?

    Console.WriteLine("길드 이름은? :");
    Console.Write("> ");
    string name = Console.ReadLine();

    // 장점 : 필요한 정보만 뺄 수 있다.
    // 단점 : 매번 필요한 정보를 코딩해야한다.

    using (var db = new AppDbContext())
    {
        var info = db.Guilds
            .Where(g => g.GuildName == name)
            .Select(g => new
            {
                Name = g.GuildName,
                MemberCount = g.Members.Count
            })
            .First();                   // 하나만 찾아주세요


        Console.WriteLine($"GuildName - {info.Name} / MemeberCount - {info.MemberCount}");
        
    }
}
}
```

---

## 코드 정리

* [Get Code 🌍](https://github.com/EasyCoding-7/MMO_EFCore_Tutorial/tree/3-3)

```csharp
// 아래단점을 보완해보자.

// 장점 : 필요한 정보만 뺄 수 있다.
// 단점 : 매번 필요한 정보를 코딩해야한다.

using (var db = new AppDbContext())
{
    var info = db.Guilds
        .Where(g => g.GuildName == name)
        .Select(g => new
        {
            Name = g.GuildName,
            MemberCount = g.Members.Count
        })
        .First();                   // 하나만 찾아주세요
```

```csharp
public static void SelectLoading()
{
    // 길드원의 수만 출력하고 싶다면?

    Console.WriteLine("길드 이름은? :");
    Console.Write("> ");
    string name = Console.ReadLine();

    // 장점 : 필요한 정보만 뺄 수 있다.
    // 단점 : 매번 필요한 정보를 코딩해야한다.

    using (var db = new AppDbContext())
    {
        var info = db.Guilds
            .Where(g => g.GuildName == name)
            /*
            // DTO를 활용가능
            .Select(g => new GuildDto
            {
                Name = g.GuildName,
                MemberCount = g.Members.Count
            })*/
            .MapGuildToDto()            // Extension을 써도된다.
            .First();                   // 하나만 찾아주세요


        Console.WriteLine($"GuildName - {info.Name} / MemeberCount - {info.MemberCount}");
        
    }
}
```

```csharp
public static class Extensions
{
    public static IQueryable<GuildDto> MapGuildToDto(this IQueryable<Guild> guild)
    {
        return guild.Select(g => new GuildDto
        {
            Name = g.GuildName,
            MemberCount = g.Members.Count
        });
    }
}
```