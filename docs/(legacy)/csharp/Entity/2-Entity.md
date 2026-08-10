---
layout: default
title: "2. CRUD 써보기"
parent: (Entity)
grand_parent: "(C#)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/EasyCoding-7/MMO_EFCore_Tutorial/tree/2-1)

## Create

```csharp
namespace MMO_EFCore_Tutorial
{
    public class DbCommands
    {
        public static void InitializedDB(bool forceReset = false)
        {
            // using을 쓸 경우 db를 생성후 자동으로 삭제(.Dispose)를 해준다.
            using (AppDbContext db = new AppDbContext())
            {
                if (!forceReset && (db.Database.GetService<IDatabaseCreator>() as RelationalDatabaseCreator).Exists())
                    return;

                db.Database.EnsureDeleted();
                db.Database.EnsureCreated();

                CreateTestData(db);
                Console.WriteLine("DB Initialized");
            }
        }

        public static void CreateTestData(AppDbContext db)
        {
            var player = new Player()
            {
                Name = "Taehyung"
            };

            List<Item> items = new List<Item>()
            {
                new Item()
                {
                    TemplateId = 101,
                    CreatedDate = DateTime.Now,
                    Owner = player
                },
                new Item()
                {
                    TemplateId = 102,
                    CreatedDate = DateTime.Now,
                    Owner = player
                },
                new Item()
                {
                    TemplateId = 103,
                    CreatedDate = DateTime.Now,
                    Owner = new Player() { Name = "New Name" }
                }
            };

            // AddRange는 여러개를 한번에 적용해 달라
            db.Items.AddRange(items);
            // SaveChanges : 반영해주세요
            db.SaveChanges();
        }
    }
}
```

🚀 여기서 주목할 점은 ItemId는 별도로 지정하지 않음을 보여주는데 PK이기에 entity에서 알아서 카운트해주길 바라여 안넣음(자동으로 됨)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/entity/entity-2-1.png"/>
</p>

---

## Read

```csharp
public static void ReadAll()
{
    using (var db = new AppDbContext())
    {
        // AsNoTracking - Read Only
            // 데이터 변경이 있으면 entity자체에서 탐지되어 반영하기에 속도가 느려진다.
        // Include - Foreign Key의 내용도 즉시 로딩해달라(안할시 {item.Owner.Name}과 같은 호출 불가
        foreach (Item item in db.Items.AsNoTracking().Include(i => i.Owner))
        {
            Console.WriteLine($"TemplateId({item.TemplateId}) Ower({item.Owner.Name}) Created({item.CreatedDate}))");
        }
    }
}
```

---

## Update

```csharp
public static void UpdateDate()
{
    Console.WriteLine("Input Player Name");
    Console.WriteLine("> ");
    string name = Console.ReadLine();

    using (var db = new AppDbContext())
    {
        var items = db.Items.Include(i => i.Owner)
                            .Where(i => i.Owner.Name == name);

        foreach (Item item in items)
        {
            item.CreatedDate = DateTime.Now;    
        }

        db.SaveChanges();
    }

    ReadAll();
}
```

---

## Delete

```csharp
public static void DeleteItem()
{
    Console.WriteLine("Input delete Player Name");
    Console.WriteLine("> ");
    string name = Console.ReadLine();

    using (var db = new AppDbContext())
    {
        var items = db.Items.Include(i => i.Owner)
                            .Where(i => i.Owner.Name == name);

        db.Items.RemoveRange(items);
        db.SaveChanges();
    }

    ReadAll();
}
```