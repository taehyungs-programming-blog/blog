---
layout: default
title: "17. 채팅 테스트 - 2"
parent: "(기초)"
grand_parent: "(GameServer C# 🎯)"
great_grand_parent: "Legacy Archive"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code](https://github.com/Arthur880708/CSharp_GameServer_Basic_Examples/tree/Tag-37-chattest-2)

* Client구현을 해보자.

```csharp
class Program
{
    static void Main(string[] args)
    {
        string host = Dns.GetHostName();
        IPHostEntry ipHost = Dns.GetHostEntry(host);
        IPAddress ipAddr = ipHost.AddressList[0];
        IPEndPoint endPoint = new IPEndPoint(ipAddr, 7777);

        Connector connector = new Connector();

        connector.Connect(endPoint, 
                        () => { return SessionManager.instance.Generate(); },
                        10 );	// Client 몇개를 시뮬레이션할지를 결정

        while (true)
        {
            try
            {
                // 채팅패킷을 쏘게 변경(테스트)
                SessionManager.instance.SendForEach();

            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }

            Thread.Sleep(250);
        }
    }
}
```

```csharp
class SessionManager
{
    static SessionManager _session = new SessionManager();
    public static SessionManager instance { get { return _session; } }


    List<ServerSession> _sessions = new List<ServerSession>(); 
    object _lock = new object();

    public ServerSession Generate()
    {
        lock(_lock)
        {
            ServerSession session = new ServerSession();
            _sessions.Add(session);
            return session;
        }
    }


    public void SendForEach()
    {
        lock (_lock)
        {
            foreach (ServerSession session in _sessions)
            {
                C_Chat chatPacket = new C_Chat();
                chatPacket.chat = $"Hello Server !";
                ArraySegment<byte> segment = chatPacket.Write();
                session.Send(segment);
            }
        }
    }
}
```