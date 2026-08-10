---
layout: default
title: "13. Socket - 6"
parent: "(C# IOCP)"
grand_parent: "Game Server 👾"
great_grand_parent: "Legacy Archive"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code](https://github.com/EasyCoding-7/CSharp_Windows_GameServer_Tutorial/tree/Tag-20-Session-4)

```csharp
namespace ServerCore
{
    // GameSession을 만들어 Connect, Recv, Send, Disconnect를 override해보자.

    class GameSession : Session
    {
        public override void OnConnected(EndPoint endPoint)
        {
            Console.WriteLine($"[OnConnected] {endPoint}");

            byte[] sendBuff = Encoding.UTF8.GetBytes("Welcom to MMORPG Server ");
            Send(sendBuff);
            Thread.Sleep(1000);
            Disconnect();
        }

        public override void OnRecv(ArraySegment<byte> buffer)
        {
            string recvData = Encoding.UTF8.GetString(buffer.Array, buffer.Offset, buffer.Count);
            Console.WriteLine($"[From Client] {recvData}");
        }

        public override void OnSend(int numOfBytes)
        {
            Console.WriteLine($"[OnSend] {numOfBytes}");
        }

        public override void OnDisconnected(EndPoint endPoint)
        {
            Console.WriteLine($"[OnDisconnected] {endPoint}");
        }
    }

    class Program
    {
        static Listener _listener = new Listener();

        static void Main(string[] args)
        {
            string host = Dns.GetHostName();
            IPHostEntry ipHost = Dns.GetHostEntry(host);
            IPAddress ipAddr = ipHost.AddressList[0];
            IPEndPoint endPoint = new IPEndPoint(ipAddr, 7777);

            // GameSession을 만들어 달라고 전달
            _listener.Init(endPoint, () => { return new GameSession(); });

            while (true)
            {
            }
        }
    }
}
```

```csharp
public class Listener
{
    Socket _listenSocket;

    // Session을 컨텐츠단에서 만들어 전달받기위함
    Func<Session> _sessionFactory;

    public void Init(IPEndPoint endPoint, Func<Session> sessionFactory)
    {
        _listenSocket = new Socket(endPoint.AddressFamily, SocketType.Stream, ProtocolType.Tcp); /* TCP로 사용하겠다는 선언 */

        // session을 전달받는다
        _sessionFactory += sessionFactory;

// ...

    void OnAcceptCompleted(object sender, SocketAsyncEventArgs args)
    {
        if (args.SocketError == SocketError.Success)
        {
            Session session = _sessionFactory.Invoke();
            session.Start(args.AcceptSocket);
            session.OnConnected(args.AcceptSocket.RemoteEndPoint);
        }
        else
        {
            Console.WriteLine(args.SocketError.ToString());
        }

        // 다음 클라를 위해서 다시 Register한다
        RegisterAccept(args);
    }
```