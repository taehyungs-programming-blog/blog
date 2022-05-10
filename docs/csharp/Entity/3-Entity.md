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