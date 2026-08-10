---
layout: default
title: "1. 환경설정 + DB연결"
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

## ORM 장점

* 개발 속도가 빨라진다
    * SQL을 직접작성하지 않아도 된다.
* 버전 관리
    * 라이브를 고려 유동적 DB 관리가 편하다
    * Migration 스크립트 자동 생성

---

## 환경설정

* [Get Code 🌍](https://github.com/EasyCoding-7/MMO_EFCore_Tutorial)

🐳 C# **.Net Core** 새 프로젝트 생성<br>
🐳 NuGet Package 👉 Microsoft.EntityFrameworkCore.SqlServer 설치<br>
🐳 프로젝트가 .Net Core 3.0이면 설치가 안될수 있는데 .Net 6.0 으로 변경해 준다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/entity/entity-1-1.png"/>
</p>

---

## DB연결

* [Get Code 🌍](https://github.com/EasyCoding-7/MMO_EFCore_Tutorial/tree/1-1)

```csharp
namespace MMO_EFCore_Tutorial
{
    internal class Program
    {
        // DB 사용 선언(받아들일 것.)
        static void InitializedDB(bool forceReset = false)
        {
            // using을 쓸 경우 db를 생성후 자동으로 삭제(.Dispose)를 해준다.
            using (AppDbContext db = new AppDbContext())
            {
                if (!forceReset && (db.Database.GetService<IDatabaseCreator>() as RelationalDatabaseCreator).Exists())
                    return;

                db.Database.EnsureDeleted();
                db.Database.EnsureCreated();

                Console.WriteLine("DB Initialized");
            }
        }

        static void Main(string[] args)
        {
            InitializedDB(forceReset: true);
            Console.WriteLine("Hello World!");
        }
    }
}
```

```csharp
// DB 모델링
// DataModel.cs

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

        // FK(Foreign Key) - 다른 클래스 참조(Navigation Property라고도 한다.)
        public Player Owner { get; set; }   
        public int OwnerId { get; set; }
    }

    public class Player
    {
        public int PlayerId { get; set; }
        public string Name { get; set; }
    }
}
```

```csharp
namespace MMO_EFCore_Tutorial
{
    /*
     * 참고) EF Core 작동 방식
     * 1) DbContext 를 만들며 DBSet<T>를 찾는다.
     * 2) DBSet<T>내의 T에서 컬럼을 찾는다 (여기선 ItemId, TemplateId, CreatedDate, Owner, OwnerId가 된다.
     * 3) 찾은 컬럼에서 참조되는 다른 클래스가 있다면 걔도 분석(여기선 Owner인 Player가 된다.)
     * 4) OnModelCreating 함수 호출
     * 5) 데이터베이스의 전체 모델링 구조를 내부 메모리에 들고 있는다.
     */


    public class AppDbContext : DbContext
    {
        public DbSet<Item> Items { get; set; }
        // 일단 DBSet을 만든 자체가 EF Core에게 Item이라는 DB를 만들생각이며 칼럼/키는 Item을 참조해 달라고 전한 것.


        // ConnectionString - Db를 어떤식으로 연결해 달라는 알림.
        public const string ConnectionString = @"Data Source=(localdb)\MSSQLLocalDB;Initial Catalog=EfCoreDB;Integrated Security=True;Connect Timeout=30;Encrypt=False;TrustServerCertificate=False;ApplicationIntent=ReadWrite;MultiSubnetFailover=False";

        protected override void OnConfiguring(DbContextOptionsBuilder options)
        {
            options.UseSqlServer(ConnectionString);
        }
    }
}
```

🐳 ConnectionString 만드는 방법<br>
🐳 SQL 개채탐색기의 SQL Server 우클릭 -> 속성

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/entity/entity-1-2.png"/>
</p>

