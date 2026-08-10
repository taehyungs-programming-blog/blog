---
layout: default
title: "5. 세부 설정"
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

## Data Modeling Config

🐥 DB를 사용시 핵심적 데이터에는 index를 걸어놔야 한다. 만약 걸지 않으면 데이터를 하나하나 순회한다<br>
🐥 **Convention** - 각종 형식과 이름을 정해진 규칙에 맞게 만들시 Entity에서 알아서 해준다.<br>
🐥 **Data Annotation** - Attribute를 붙여 추가 정보를 제공<br>
🐥 **Fluent Api** - `OnModelCreating`에서 직접 설정을 정의<br>

```csharp
[Table("Player")]
public class Player
{
    // 클래스의 이름이 Player인데 PlayerId로 만들시 Convention이 자동으로 적용된다.(Primary Key)
        // 단, public + non-static 여야 한다.
    public int PlayerId { get; set; }

    public string Name { get; set; }

    public Item Item { get; set; }

    public Guild Guild { get; set; }

    /*
    * Convention 특징
        * property이름이 table column의 이름이 된다.
        * .NET의 데이터 타입이 SQL의 데이터 타입이 된다.(int, bool)
        * .NET의 Nullable을 따라간다.(int?, bool?)
        * {class명}Id - Primary Key로 인식하게 된다.
        * 복합키(Composite Key)는 Convention으로 처리가 안된다.
    */
}
```

```csharp
protected override void OnModelCreating(ModelBuilder builder)
{
    // Fluent Api의 예시 OnModelCreating에서 필터링
    builder.Entity<Item>().HasQueryFilter(i => i.SoftDeleted == false);
}
```

* 그 외 Data Annotation, Fluent Api를 살펴보자면
    * Nullable 표현
        * Data Annotation - `[Required]`
        * Fluent Api - `IsRequired()`
    * 문자열 길이 제한
        * Data Annotation - `[MaxLength(20)]`
        * Fluent Api - `HasMaxLength(20)`
    * 문자형식 제한
        * Fluent Api - `IsUnicode(true)`
    * Primary Key
        * Data Annotation - `[Key][Column(Order=0)]`, `[Key][Column(Order=1)]`
        * Fluent Api - `.HasKey(x => new (x.Prop1, x.Prop2))`
    * Index
        * Fluent Api - `.HasIndex(p => p.Prop1)`
    * Column 이름
        * Data Annotation - `[Column("이름")`
        * Fluent Api - `.HasColumnName("이름")`

🐥 ... 모두 정리하긴 곤란하고 필요할 경우 찾아보도록 하자.<br>
🐥 기본적인것은 Convention으로 처리, Validation관련은 Data Annotation으로 하는걸 추천(하나로 모아준다)

---

## Relataionship Config

🐥 **Principal Entity** - 1:1 관계에서 주요 Entity(아래참조)<Br>
🐥 **Dependent Entity** - 1:1 관계에서 의존 Entity(아래참조)<Br>

```csharp
// Item과 Player는 1:1 관계이다.

[Table("Item")]
public class Item
{
    public bool SoftDeleted { get; set; }

    // PK(Primary Key)
    public int ItemId { get; set; }
    public int TemplateId { get; set; }
    public DateTime CreatedDate { get; set; }

    // Item이 Player를 ForeignKey로 물고있기에
    // Item이 Dependent Entity이고 Player가 Principal Entity가 된다.
    //[ForeignKey("OwnerId")] 
    public int? OwnerId { get; set; }
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
```

🐥 **Navigational Property** - 참조형으로 다른 Entity를 들고있는 것을 말함(Item기준 Player가 Navigation Property)<Br>
🐥 **Primary Key** - 하나 뿐인 대표키<Br>
🐥 **Foreign Key** - 외부를 가리키는 키<Br>
🐥 **Principal Key** - Primary Key or Unique Alternate Key<Br>
🐥 **Required Relationship** - 관계되는 Entity가 Not-null(null이면 절대 안됨)<Br>
🐥 **Optional Relationship** - 관계되는 Entity가 Nullable 허용<Br>

* Convention방식으로 **Foreign Key** 설정방법
    * `{PrincipalKeyName}` 👉 `PlayerId`
    *  `{class}{PrincipalKeyName}` 👉 `PlayerPlayerId`
    *  `{NavigationPropertyName}{PrincipalKeyName}` 👉 `OwnerPlayerId` or `OwnerId`
* **Foreign Key와 Nullable** 다시 정리(중요)
    * Required Relationship (Not-Null)
        * 삭제할 때 OnDelete 인자를 Cascade 모드로 호출 -> Principal 삭제하면 Dependent 삭제 됨
    * Optional Relationship (Nullable)
        * Principal 삭제할 때 Dependent Tracking 하고 있으면 FK를 null로 세팅
        * Principal 삭제할 때 Dependent Tracking 하고 있지 않다면 Exception 발생
* Cenvention 방식으로 못하는 것
    * 복합 Foreign Key
    * 다수의 Navigation Property가 같은 클래스 참조(아래의 케이스)
    * DB나 삭제관련 커스터 마이징

🐥 궁금증 아래와 같은 경우는 어떻게 될까?

```csharp
[Table("Item")]
public class Item
{
    // ...

    // Player를 두 개의 Entity에서 참조할 경우?
    // ForeignKey를 어떻게 선언할까? -> 일단 Convention은 안됨.
    public int OwnerId { get; set; }
    public Player Owner { get; set; }   

    public int CreatorId { get; set; }
    public Player Creator { get; set; }   
}


[Table("Player")]
public class Player
{
    // ...
    
    public Item OwnedItem { get; set; }
    public ICollection<Item> CreatedItems { get; set; }
    
    // ...
}
```

🐥 해결해 보자.

```csharp
[Table("Item")]
public class Item
{
    // ...

    public int OwnerId { get; set; }
    public Player Owner { get; set; }   

    public int? CreatorId { get; set; }
    public Player Creator { get; set; }   
}


[Table("Player")]
public class Player
{
    // ...
    
    [InverseProperty("Owner")]  // 짝꿍이 누구인지 직접알려준다
    public Item OwnedItem { get; set; }

    [InverseProperty("Creator")]
    public ICollection<Item> CreatedItems { get; set; }
    
    // ...
}
```

🐥 물론 Fluent Api로도 해결이 가능하다 -> `.HasOne()`, `.HasMany()` ... (찾아보도록 하자)

---

## Shadow Property & Backing Field

### Shadow Property

🐥 class에는 있지만 DB에 넣고 싶지 않다면? `[NotMapped]` or `.Ignore()`<br>
🐥 **Shadow Property** - 반대로 DB에는 있지만 class에는 없는 경우<br>
🐥 언제쓰나? 👉 Item을 복구한 날자와 같은경우는 DB에 직접접근해서 쓸 일은 있지만 Entity로서 사용할 경우는 실수를 제외하면 없을 것이다.

```csharp
protected override void OnModelCreating(ModelBuilder builder)
{
    // Shadow Property 만들기
    builder.Entity<Item>().Property<Datetime>("RecoveredData");
}
```

### Backing Field

🐥 private property를 DB에 매핑, public getter로 가공해서 사용<br>
🐥 언제쓰나? 👉 DB에 json형태의 string으로 저장 후 getter는 json을 가공해서 받고 싶은경우

```csharp
public struct ItemOption
{
    public int str;
    public int dex;
    public int hp;
}

[Table("Item")]
public class Item
{
    private string _jsonData;
    public string JsonData
    {
        get { return _jsonData; }
        set { _jsonData = value; }
    }

    public void SetOption(ItemOption option)
    {
        _jsonData = JsonConvert.SerializedObject(option);
    }

    public ItemOption GetOption()
    {
        return JsonConvert.DesertializeObject<ItemOption>(_jsonData);
    }

    // ...
}

// Test Backing Field
items[0].SetOption(new ItemOption() {dex =1, hp=2, str=3});
```

```csharp
protected override void OnModelCreating(ModelBuilder builder)
{
    // Backing Field
    builder.Entity<Item>()
        .Property(i => i.JsonData)
        .HasField("_jsonData");
}
```

---

## Entity Class & Table Mapping

🐥 Entity와 DB 연동, 현재는 Entity 하나를 통으로 로드한다(부담)<br>
🐥 **Owned Type** - 일반 class를 Entity class에 추가<br>

```csharp
public struct ItemOption
{
    public int Str { get; set; };
    public int Dex { get; set; };
    public int Hp { get; set; };
}

[Table("Item")]
public class Item
{
    public ItemOption Option { get; set; }

    // 이런방식말고
    /*
        // 마치 코드를 직접넣는 방법(참조로 갖지말고 직접 갖는다)은 없을까? -> Owned Type
        public int Str { get; set; };
        public int Dex { get; set; };
        public int Hp { get; set; };
    */

    // ...
```

```csharp
protected override void OnModelCreating(ModelBuilder builder)
{
    builder.Entity<Item>()
        .OwnsOne(i => i.Option);
}
```

```csharp
protected override void OnModelCreating(ModelBuilder builder)
{
    builder.Entity<Item>()
        .OwnsOne(i => i.Option)
        .ToTable("ItemOption"); // 같은 테이블말고 다른테이블에 만들어 줘
}
```

🐥 **Table Per Hierarchy** - 상속 관계의 여러 클래스를 하나의 테이블에 매핑<br>
🐥 Animal기반의 Dog, Cat ... 을 하나의 Table에서 관리하고자 할때<br>

```csharp
[Table("Item")]
public class Item
{
    // ...
}

public class EventItem : Item
{
    public DateTime DestroyDate { get; set; }
}
```

```csharp
    public class AppDbContext : DbContext
    {
        public DbSet<Item> Items { get; set; }
        // covention방식은 그냥 DbSet에 추가만 해주면 자동으로 된다.
            // EventItems라는 Entity가 생기는게 아니라 Items내에 들어가게 된다.
        public DbSet<EventItem> EventItems { get; set; }
        public DbSet<Player> Players { get; set; }
        public DbSet<Guild> Guilds { get; set; }

// 이 방식의 문제는 Items에 접근할때 EventItems에 접근할때 별도로 Entity에 접근해야한다.
// Items에 접근시 Items Entity / EventItems에 접근시 EventItems Entity
```

🐥 문제를 해결해보자.

```csharp
public enum ItemType
{
    NormalItem,
    EventItem
}

[Table("Item")]
public class Item
{
    public ItemType Type { get; set; }
    // ...
}

public class EventItem : Item
{
    public DateTime DestroyDate { get; set; }
}
```

```csharp
protected override void OnModelCreating(ModelBuilder builder)
{
    // TPH
    builder.Entity<Item>()
        .HasDiscriminator(i => i.Type)
        .HasValue<Item>(ItemType.NormItem)
        .HasValue<EventItem>(ItemType.EventItem)
}
```

🐥 **Table Splitting** - 다수의 Entity를 하나의 테이블에 매핑<br>

```csharp
public class ItemDetail
{
    public int ItemDetailId { get; set; }
    public string Description { get; set; }
}

[Table("Item")]
public class Item
{
    public ItemDetail Detail { get; set; }
    // ...
}
```

```csharp
protected override void OnModelCreating(ModelBuilder builder)
{
    // Table Splitting
    builder.Entity<Item>()
        .HasOne(i => i.Detail)
        .WithOne()
        .HasForeignKey<ItemDetail>(i => i.ItemDetailId);

    builder.Entity<Item>().ToTable("Items");
    builder.Entity<ItemDetail>().ToTable("Items");
}
```

---

## Backing Field, Relationship

🐥 **Backing Field** - private field를 DB에 매핑, Navigation Property에도 사용이가능

```csharp
// 사용자가 평가하는 아이템의 리뷰를 남긴다 가정해보자.

public class ItemReview
{
    public int ItemReviewId { get; set; }
    public int Score { get; set; }
}

[Table("Item")]
public class Item
{
    // 단순 평점만 보는게 아니라 평점의 평균을 보고 싶다면??
    public double? AverageScore { get; set; }   // 이걸 어느 타이밍에 계산해야할까? 매번 스코어 확인시 계산은 비효율적일듯 한데...
    public ICollection<ItemReview> Reviews { get; set; }
    // ...
}
```

```csharp
[Table("Item")]
public class Item
{
    public double? AverageScore { get; set; }

    private readonly List<ItemReview> _reviews = new List<ItemReview>();
    public IEnumerable<ItemReview> Reviews { get{ return _reviews.ToList(); } }

    public void AddReview(ItemReview review)
    {
        _reviews.Add(review)
        AverageScore = _reviews.Average(r => r.Score);
    }

    public void RemoveReview(ItemReview review)
    {
        _reviews.Remove(review);
        AverageScore = _reviews.Any() ? _reviews.Average(r => r.Score) : (double?)null;
    }
    // ...
}
```

```csharp
protected override void OnModelCreating(ModelBuilder builder)
{
    // 여기는 이렇게 쓴다고 받아들이자.(필요하다면 찾아봐도 무방함)
    builder.Entity<Item>()
        .Metadata
        .FindNavigation("Reviews")
        .SetPropertyAccessMode(PropertyAccessMode.Field);
}
```

---

## User Defined Function(UDF)

🐥 만들어진 SQL을 호출하는 방법? AND 연산을 DB에서 하고싶을경우?

1. Configuration
2. Database Setup
3. 사용

```csharp
// 같은 예제로 리뷰의 평균을 DB에서 계산하게 해보자.

public class ItemReview
{
    public int ItemReviewId { get; set; }
    public int Score { get; set; }
}

[Table("Item")]
public class Item
{
    public ICollection<ItemReview> Reviews { get; set; }

    // ...
}
```

```csharp
public static double? GetAverageReviewScore(int itemId)
{
    [DbFunction()]
    throw new NotImplementedException("C#에서 사용시 호출 됨");
}

// or

protected override void OnModelCreating(ModelBuilder builder)
{
    // DbFunction
    builder.HasDbFunction(() => Program.GetAverageReviewScore(0));
}
```

🐥 여기까지가 Configuration이고 이제 Database Setup

```csharp
public static void InitializedDB(bool forceReset = false)
{
    using (AppDbContext db = new AppDbContext())
    {
        if (!forceReset && (db.Database.GetService<IDatabaseCreator>() as RelationalDatabaseCreator).Exists())
            return;

        db.Database.EnsureDeleted();
        db.Database.EnsureCreated();

        // 여기서 SQL문법을 넣자
        string command = 
                @" CREATE FUNCTION getAverageReviewScore (@item INT) RETURN FLOAT
                    AS
                    BEGIN
                    DECLARE @result AS FLOAT

                    SELECT @result = AVG(CAST([Score] AS FLOAT))
                    FROM ItemReiew AS r
                    WHERE @itemId = r.ItemId

                    RETURN @result

                    END";

        db.Database.ExecuteSqlRaw(command);

        // Create
        CreateTestData(db);
        Console.WriteLine("!!! DB is Reset !!!");
    }
}
```

---

## Default Value(초기값)

* Auto-Property Initializer(C# 6.0)
    * (주의) DB에 초기값이 붙는게 아니라 Entity에만 초기값이 붙음.
    * `SaveChange()`를 호출해 줘야한다.

```csharp
[Table("Item")]
public class Item
{
    // Auto-Property Initializer
    public DateTime CreateDate { get; set; } = new DateTime(2020, 1, 1);

    // ...
}
```

* Fluent Api
    * (주의) DB에 초기값이 붙음.
    * `SaveChange()`가 초기에 없어도 된다.

```csharp
protected override void OnModelCreating(ModelBuilder builder)
{
    builder.Entity<Item>()
        .Property("CreateDate")
        // .HasDefaultValue(new DateTime(2020, 1, 1));
        .HasDefaultValue(DateTime.Now);     // (문제) 현재시간이 아니라 코드가 실행된 시간이 들어가게 된다.
}
```

* SQL Fragment
    * 새로운 값이 추가되는 시점에 DB에서 실행

```csharp
[Table("Item")]
public class Item
{
    public DateTime CreateDate { get; private set; }

    // ...
}
```

```csharp
protected override void OnModelCreating(ModelBuilder builder)
{
    builder.Entity<Item>()
        .Property("CreateDate")
        .HasDefaultValueSql("GETDATE()");
}
```

* Value Generator

```csharp
public class PlayerNameGenerator : ValueGenerator<string>
{
    public override bool GeneratesTemporaryValues => false;

    public override string Next(EntityEntry entry)
    {
        string name = $"Player {DateTime.Now.ToString("yyyyMMdd")};
        return name;
    }
}
```

```csharp
protected override void OnModelCreating(ModelBuilder builder)
{
    builder.Entity<Player>()
        .Property(p => p.Name)
        .HasValueGenerator((p, e) => new PlayerNameGenerator());
}
```