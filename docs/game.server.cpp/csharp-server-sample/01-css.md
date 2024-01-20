---
layout: default
title: "01. 패킷생성 및 DB써보기"
parent: "(C# Server Sample)"
grand_parent: "Game Server 👾"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/kthCodeLog/csharp-gameserver-sample/tree/01)

## DB 연동

```csharp
namespace Server.DB
{
	public class AppDbContext : DbContext
	{
		public DbSet<AccountDb> Accounts { get; set; }
		public DbSet<PlayerDb> Players { get; set; }

        // DB LoggerFactory - DB Log를 Console에 찍는데 사용된다.
		static readonly ILoggerFactory _logger = LoggerFactory.Create(builder => { builder.AddConsole(); });

		string _connectionString = @"/blabla~";

		protected override void OnConfiguring(DbContextOptionsBuilder options)
		{
			options
                // console log연결
				.UseLoggerFactory(_logger)

                // ConfigManager에 json으로 설정파일이 있다면 로드
				.UseSqlServer(ConfigManager.Config == null ? 
                            _connectionString : 
                            ConfigManager.Config.connectionString);
		}

		protected override void OnModelCreating(ModelBuilder builder)
		{
            // Name으로 검색할 예정 index를 Name으로 건다.
			builder.Entity<AccountDb>()
				.HasIndex(a => a.AccountName)
				.IsUnique();

			builder.Entity<PlayerDb>()
				.HasIndex(p => p.PlayerName)
				.IsUnique();
		}
	}
}
```

```csharp
		public void HandleLogin(C_Login loginPacket)
		{
			// ...

            // (참고) db를 사용시 자동으로 위가 적용됨.
			using (AppDbContext db = new AppDbContext())
			{
```

```csharp
// 데이터 구성은 우선 이렇게
namespace Server.DB
{
	[Table("Account")]
	public class AccountDb
	{
		public int AccountDbId { get; set; }
		public string AccountName { get; set; }
		public ICollection<PlayerDb> Players { get; set; }
	}

	[Table("Player")]
	public class PlayerDb
	{
		public int PlayerDbId { get; set; }
		public string PlayerName { get; set; }

		[ForeignKey("Account")]
		public int AccountDbId { get; set; }
		public AccountDb Account { get; set; }

		public int Level { get; set; }
		public int Hp { get; set; }
		public int MaxHp { get; set; }
		public int Attack { get; set; }
		public float Speed { get; set; }
		public int TotalExp { get; set; }
	}
}
```

---

## Player 접속

```csharp
// protocol.proto에 접속에 사용되는 메시지를 등록한다.

enum MsgId {
  S_ENTER_GAME = 0;
  S_LEAVE_GAME = 1;
  S_SPAWN = 2;
  S_DESPAWN = 3;
  C_MOVE = 4;
  S_MOVE = 5;
  C_SKILL = 6;
  S_SKILL = 7;
  S_CHANGE_HP = 8;
  S_DIE = 9;

  // 접속에 사용될 예정
  S_CONNECTED = 10;
  C_LOGIN = 11;
  S_LOGIN = 12;
  C_ENTER_GAME = 13;
  C_CREATE_PLAYER = 14;
  S_CREATE_PLAYER = 15;
}

// GenProto.bat을 통해 코드생성 잊지말기
```

```csharp
public void Register()
{		
    _onRecv.Add((ushort)MsgId.CMove, MakePacket<C_Move>);
    _handler.Add((ushort)MsgId.CMove, PacketHandler.C_MoveHandler);		
    _onRecv.Add((ushort)MsgId.CSkill, MakePacket<C_Skill>);
    _handler.Add((ushort)MsgId.CSkill, PacketHandler.C_SkillHandler);		

    // 추가된 패킷정의
    _onRecv.Add((ushort)MsgId.CLogin, MakePacket<C_Login>);
    _handler.Add((ushort)MsgId.CLogin, PacketHandler.C_LoginHandler);		
    _onRecv.Add((ushort)MsgId.CEnterGame, MakePacket<C_EnterGame>);
    _handler.Add((ushort)MsgId.CEnterGame, PacketHandler.C_EnterGameHandler);		
    _onRecv.Add((ushort)MsgId.CCreatePlayer, MakePacket<C_CreatePlayer>);
    _handler.Add((ushort)MsgId.CCreatePlayer, PacketHandler.C_CreatePlayerHandler);
}
```

```csharp
// 다른 부분은 그냥 코드를 보고 쓰면되지만
// db와 관련된 부분을 어떻게 사용할지가 핵심.

public void HandleLogin(C_Login loginPacket)
{
    if (ServerState != PlayerServerState.ServerStateLogin)
        return;

    LobbyPlayers.Clear();

    // DB를 읽을준비
    using (AppDbContext db = new AppDbContext())
    {
        // 계정정보를 읽는다
        AccountDb findAccount = db.Accounts
            .Include(a => a.Players)
            .Where(a => a.AccountName == loginPacket.UniqueId).FirstOrDefault();

        if (findAccount != null)
        {
            // AccountDbId 메모리에 기억
            AccountDbId = findAccount.AccountDbId;

            S_Login loginOk = new S_Login() { LoginOk = 1 };
            foreach (PlayerDb playerDb in findAccount.Players)
            {
                LobbyPlayerInfo lobbyPlayer = new LobbyPlayerInfo()
                {
                    Name = playerDb.PlayerName,
                    StatInfo = new StatInfo()
                    {
                        Level = playerDb.Level,
                        Hp = playerDb.Hp,
                        MaxHp = playerDb.MaxHp,
                        Attack = playerDb.Attack,
                        Speed = playerDb.Speed,
                        TotalExp = playerDb.TotalExp
                    }
                };

                // 메모리에도 들고 있다
                LobbyPlayers.Add(lobbyPlayer);

                // 패킷에 넣어준다
                loginOk.Players.Add(lobbyPlayer);
            }

            Send(loginOk);
            // 로비로 이동
            ServerState = PlayerServerState.ServerStateLobby;
        }
        else
        {
            AccountDb newAccount = new AccountDb() { AccountName = loginPacket.UniqueId };
            db.Accounts.Add(newAccount);
            db.SaveChanges(); // TODO : Exception 

            // AccountDbId 메모리에 기억
            AccountDbId = newAccount.AccountDbId;

            S_Login loginOk = new S_Login() { LoginOk = 1 };
            Send(loginOk);
            // 로비로 이동
            ServerState = PlayerServerState.ServerStateLobby;
        }
    }
}
```