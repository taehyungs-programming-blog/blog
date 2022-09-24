---
layout: default
title: "09. Socket - 2"
parent: "(C# IOCP)"
grand_parent: "Game Server 👾"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code](https://github.com/EasyCoding-7/CSharp_Windows_GameServer_Tutorial/tree/Tag-16-ListenerClass)


🍁 우선 DummyClient를 건드리지 않고 Server에 몇가지 개선을 해볼 예정<br>
🍁 Listner class를 통해 Accept를 동작을 별도로 분리할 예정

```csharp
// ServerCore
// Program.cs

namespace ServerCore
{
    class Program
    {
        // Listner class를 만들어 Listen처리를 할 예정
        static Listener _listener = new Listener();

        static void OnAcceptHandler(Socket clientSocket)
        {
            try
            {
                // 받는 부분
                byte[] recvBuff = new byte[1024];
                int recvBytes = clientSocket.Receive(recvBuff);
                string recvData = Encoding.UTF8.GetString(recvBuff, 
                                                        0/*시작인덱스*/, 
                                                        recvBytes/*문자열이 몇개인가*/);

                Console.WriteLine($"[From Client] {recvData}");

                // 보내는 부분
                byte[] sendBuff = Encoding.UTF8.GetBytes("Welcom to MMORPG Server ");
                clientSocket.Send(sendBuff);

                // 소켓을 닫는다
                clientSocket.Shutdown(SocketShutdown.Both); /*소켓을 닫는다를 미리 공지*/
                clientSocket.Close();   /*실제로 닫는 부분*/
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
            }
        }

        static void Main(string[] args)
        {
            // DNS(Domain Name System)을 사용할 예정
            // ip addr를 안쓰고 도메인네임을 쓰겠다는 말.
            string host = Dns.GetHostName();
            IPHostEntry ipHost = Dns.GetHostEntry(host);
            IPAddress ipAddr = ipHost.AddressList[0];
            IPEndPoint endPoint = new IPEndPoint(ipAddr, 7777/* Port */);

            _listener.Init(endPoint, OnAcceptHandler);

            while (true)
            {
            }
        }
    }
}
```

```csharp
// ServerCore
// Listner.cs

namespace ServerCore
{
    public class Listener
    {
        Socket _listenSocket;
        Action<Socket> _onAcceptHandler;

        public void Init(IPEndPoint endPoint, Action<Socket> onAcceptHandler)
        {
            _listenSocket = new Socket(endPoint.AddressFamily,
                                        SocketType.Stream, 
                                        ProtocolType.Tcp); /* TCP로 사용하겠다는 선언 */

            // Accept시 델리게이트 선언
            _onAcceptHandler += onAcceptHandler;

            _listenSocket.Bind(endPoint);

            _listenSocket.Listen(10);
            // 10 : 최대 대기수, 초과시 입장을 제한한다

            // for(int i = 0; i < 10; i++) // 이렇게 리스너를 여러개 두어 동시에 여러 유저의 입장을 처리가능
            SocketAsyncEventArgs args = new SocketAsyncEventArgs();
            args.Completed += new EventHandler<SocketAsyncEventArgs>(OnAcceptCompleted);
            // socket의 동작이 complete될 시 OnAcceptCompleted가 호출되게 선언
            RegisterAccept(args);
        }

        void RegisterAccept(SocketAsyncEventArgs args)
        {
            // 기존 소켓정보를 날린다
            args.AcceptSocket = null;

            bool pending = _listenSocket.AcceptAsync(args);
            // socket의 동작은 accept로 한다.
            if (pending == false)
            {
                // 바로 완료됨
                OnAcceptCompleted(null, args);
            }
        }

        void OnAcceptCompleted(object sender, SocketAsyncEventArgs args)
        {
            if (args.SocketError == SocketError.Success)
            {
                _onAcceptHandler.Invoke(args.AcceptSocket);
            }
            else
            {
                Console.WriteLine(args.SocketError.ToString());
            }

            // 다음 클라를 위해서 다시 Register한다
            RegisterAccept(args);
        }
    }
}
```