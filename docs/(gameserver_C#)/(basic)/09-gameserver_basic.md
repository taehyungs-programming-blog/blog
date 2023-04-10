---
layout: default
title: "09. Connector Class"
parent: "(basic)"
grand_parent: "(GameServer C# 🎯)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code](https://github.com/Arthur880708/CSharp_GameServer_Basic_Examples/tree/Tag-21-Connector)

* Listener는 Server용 Connector는 Client용

```csharp
// Server
// Program.cs

class Program
{
    static void Main(string[] args)
    {
        string host = Dns.GetHostName();
        IPHostEntry ipHost = Dns.GetHostEntry(host);
        IPAddress ipAddr = ipHost.AddressList[0];
        IPEndPoint endPoint = new IPEndPoint(ipAddr, 7777/* Port */);

        // Connect Class 생성
        Connector connector = new Connector();
        connector.Connect(endPoint, () => { return new GameSession(); });

        while(true)
        {
            Thread.Sleep(100);
        }
    }
}
```

```csharp
namespace ServerCore
{
    public class Connector
    {
        Func<Session> _sessionFactory;

        public void Connect(IPEndPoint endPoint, Func<Session> sessionFactory)
        {
            Socket socket = new Socket(endPoint.AddressFamily, SocketType.Stream, ProtocolType.Tcp);

            SocketAsyncEventArgs args = new SocketAsyncEventArgs();
            args.Completed += OnConnectComplete;
            args.RemoteEndPoint = endPoint;

            _sessionFactory += sessionFactory;

            // UserToken를 이용해서 원하는 데이터를 넘길 수 있음
            args.UserToken = socket;

            RegisterConnect(args);
        }

        void RegisterConnect(SocketAsyncEventArgs args)
        {
            Socket socket = args.UserToken as Socket;
            if (socket == null)
                return;

            bool pending = socket.ConnectAsync(args);
            if (pending == false)
                OnConnectComplete(null, args);
        }

        void OnConnectComplete(object sender, SocketAsyncEventArgs args)
        {
            if(args.SocketError == SocketError.Success)
            {
                Session session = _sessionFactory.Invoke();
                session.Start(args.ConnectSocket);
                session.OnConnected(args.RemoteEndPoint);
            }
            else
            {
                Console.WriteLine($"OnConnect Completed Fail : {args.SocketError}");
            }
        }
    }
}
```