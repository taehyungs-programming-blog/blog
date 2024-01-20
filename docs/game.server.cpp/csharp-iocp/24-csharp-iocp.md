---
layout: default
title: "24. 채팅 테스트 - 1"
parent: "(C# IOCP)"
grand_parent: "Game Server 👾"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code](https://github.com/EasyCoding-7/CSharp_Windows_GameServer_Tutorial/tree/Tag-36-chattest-1)

🍁 client로 부터 chat내용이 도착하면, server가 접속된 모든 client에게 broadcast해주는 식으로 동작하게 만들어 보자.

```xml
<!-- 패킷을 재정의 -->
<?xml version="1.0" encoding="utf-8" ?>
<PDL>
  <packet name="C_Chat">
    <string name="chat"/>
  </packet>
  <packet name="S_Chat">
    <int name="playerId"/>
    <string name="chat"/>
  </packet>
</PDL>
```

🍁 **Common\Packet\GenPacket.bat**을 실행하자<br>
🍁 여기까지하면 패킷은 완성

---

🍁 우선 서버를 만들어 볼 예정

```csharp
class Program
{
    static Listener _listener = new Listener();

    // 전역으로 Room을 관리한다.
    public static GameRoom Room = new GameRoom();

    static void Main(string[] args)
    {
        PacketManager.Instance.Register();

        string host = Dns.GetHostName();
        IPHostEntry ipHost = Dns.GetHostEntry(host);
        IPAddress ipAddr = ipHost.AddressList[0];
        IPEndPoint endPoint = new IPEndPoint(ipAddr, 7777);

        // SessionManager를 통해 session을 생성하게 변경
        _listener.Init(endPoint, () => { return SessionManager.instance.Generate(); });
```

```csharp
// SessionManager를 통해 session 생성 및 검색을 지원한다.

class SessionManager
{
    // singleton
    static SessionManager _session = new SessionManager();
    public static SessionManager instance { get { return _session; } }


    int _sessionId = 0;
    Dictionary<int, ClientSession> _sessions = new Dictionary<int, ClientSession>();
    object _lock = new object();

    // Client 접속시 Generate를 통해 Session을 발급한다.
    public ClientSession Generate()
    {
        lock (_lock)
        {
            int sessionId = ++_sessionId;

            ClientSession session = new ClientSession();
            session.SessionID = sessionId;
            _sessions.Add(sessionId, session);

            Console.WriteLine($"Connected : {sessionId}");

            return session;
        }
    }

    // Find는 아직은 미사용
    public ClientSession Find(int id)
    {
        lock (_lock)
        {
            ClientSession session = null;
            _sessions.TryGetValue(id, out session);

            Console.WriteLine($"Find Session : {id}");

            return session;
        }
    }

    // Room을 나갈시 Remove사용
    public void Remove(ClientSession session)
    {
        lock (_lock)
        {
            _sessions.Remove(session.SessionID);

            Console.WriteLine($"Remove Session : {session.SessionID}");

        }
    }
}
```

```csharp
class GameRoom
{
    List<ClientSession> _sessions = new List<ClientSession>();
    object _lock = new object();
    public void Enter(ClientSession session)
    {
        lock(_lock)
        {
            _sessions.Add(session);
            session.Room = this;
        }
    }

    public void Leave(ClientSession session)
    {
        lock (_lock)
        {
            _sessions.Remove(session);
        }
    }

    public void Broadcast(ClientSession session, string chat)
    {
        S_Chat packet = new S_Chat();
        packet.playerId = session.SessionID;
        packet.chat = chat;

        ArraySegment<byte> segment = packet.Write();

        lock(_lock)
        {
            foreach(ClientSession sessions in _sessions)
            {
                sessions.Send(segment);
            }
        }
    }
}
```

* `SessionManager`
    * ClientSession 생성/삭제를 담당
* `ClientSession`
    * Packet의 Connect, Recv, Send를 처리
* `GameRoom`
    * 하나의 서버라 생각하자
    * Enter, Leave, Braodcast를 담당
