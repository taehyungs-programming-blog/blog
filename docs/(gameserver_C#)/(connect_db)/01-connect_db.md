---
layout: default
title: "01. 환경설정"
parent: "(컨텐츠 제작 2 - DB 연결)"
grand_parent: "(GameServer C# 🎯)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/CSharp_GameServer_Connect_To_Db/tree/1)

## DB 연동 준비

* Nuget Package 설치
    - EntityFrameworkCore.sql 설치
    - EntityFrameworkCore.Tools 설치
    - Logging.Console 설치

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/game-server-csharp/db-connect/dbc-1-1.png"/>
</p>

---

### Tips) sql server 개체 탐색기가 보이지 않는 현상?

- visual studio installer에서 sql 관련 서비스를 다운 받아야함
- 근데 visual studio install를 켜보면 뭘 설치해야하나 싶음;;
- visual studio에서 검색 -> 서버 탐색기 -> sql server 추가 클릭 -> visual studio installer에서 뭘 설치해라고 뜨는데 그걸 그냥 설치하면 된다.

---

### Tips2) DB 연결 속성 설정

- config.json과 AppDbContex.cs의 _connectionString을 DB연결속성을 넣어준다
- 주의할 점은 Initial Catalog=GameDB만 추가해 주자!
- 주의할 점 2 `\\` 두개 들어가면 안됨 `\` 하나임

---

## DB Migration

- 패키지관리자 콘솔에서
- `$ add-migration init`
- `$ update-database`

---

```csharp
static void Main(string[] args)
{
    ConfigManager.LoadConfig();
    DataManager.LoadData();

    using (AppDbContext db = new AppDbContext())
    {
        db.Accounts.Add(new AccountDb() { AccountName = "TestAccount"});
        db.SaveChanges();
    }
```

```csharp
namespace Server.DB
{
	public class AppDbContext : DbContext
	{
		public DbSet<AccountDb> Accounts { get; set; }
		public DbSet<PlayerDb> Players { get; set; }

		static readonly ILoggerFactory _logger = LoggerFactory.Create(builder => { builder.AddConsole(); });

		string _connectionString = @"Data Source=DESKTOP-Q6TGQ9I\SQLEXPRESS;Integrated Security=True;Connect Timeout=30;Encrypt=False;TrustServerCertificate=False;ApplicationIntent=ReadWrite;MultiSubnetFailover=False;Initial Catalog=GameDB;";

		protected override void OnConfiguring(DbContextOptionsBuilder options)
		{
			options
				.UseLoggerFactory(_logger)
				.UseSqlServer(ConfigManager.Config == null ? _connectionString : ConfigManager.Config.connectionString);
		}

		protected override void OnModelCreating(ModelBuilder builder)
		{
            // 첫 생성시
			builder.Entity<AccountDb>()
				.HasIndex(a => a.AccountName)
				.IsUnique();        // AccountName을 Unique, index로

			builder.Entity<PlayerDb>()
				.HasIndex(p => p.PlayerName)
				.IsUnique();
		}
	}
}
```