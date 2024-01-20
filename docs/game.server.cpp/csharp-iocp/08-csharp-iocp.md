---
layout: default
title: "08. Socket - 1"
parent: "(C# IOCP)"
grand_parent: "Game Server 👾"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code](https://github.com/EasyCoding-7/CSharp_Windows_GameServer_Tutorial/tree/Tag-15-SocketProgramming)

🍁 아주간단한 string을 주고받는 서버/클라이언트

```csharp
// ServerCore
// Program.cs

class Program
{
    static void Main(string[] args)
    {
        // DNS(Domain Name System)을 사용할 예정
        // ip addr를 안쓰고 도메인네임을 쓰겠다는 말.
        string host = Dns.GetHostName();
        IPHostEntry ipHost = Dns.GetHostEntry(host);
        IPAddress ipAddr = ipHost.AddressList[0];
        IPEndPoint endPoint = new IPEndPoint(ipAddr, 7777/* Port */);

        Socket listenSocket = new Socket(endPoint.AddressFamily, 
                                        SocketType.Stream, 
                                        ProtocolType.Tcp); /* TCP로 사용하겠다는 선언 */
                                        // 우선 옵션은 받아들이자.

        try
        {
            listenSocket.Bind(endPoint);

            listenSocket.Listen(10);    // 10 : 최대 대기수, 초과시 입장을 제한한다

            while(true)
            {
                // 에코서버로 구현예정
                Console.WriteLine("Listening...");

                // 클라이언트가 입장하지 않으면 여기서 블락됨.( ** 나중에 개선예정 ** )
                Socket clientSocket = listenSocket.Accept();

                // 받는 부분
                byte[] recvBuff = new byte[1024];
                int recvBytes = clientSocket.Receive(recvBuff);
                string recvData = Encoding.UTF8.GetString(recvBuff, 0/*시작인덱스*/, 
                                                           recvBytes/*문자열이 몇개인가*/);

                Console.WriteLine($"[From Client] {recvData}");

                // 보내는 부분
                byte[] sendBuff = Encoding.UTF8.GetBytes("Welcom to MMORPG Server ");
                clientSocket.Send(sendBuff);

                // 소켓을 닫는다
                clientSocket.Shutdown(SocketShutdown.Both); /*소켓을 닫는다를 미리 공지*/
                clientSocket.Close();   /*실제로 닫는 부분*/
            }
        }
        catch(Exception e)
        {

        }
    }
}
```

---

```csharp
// DummyClient
// Program.cs

class Program
{
    static void Main(string[] args)
    {
        string host = Dns.GetHostName();
        IPHostEntry ipHost = Dns.GetHostEntry(host);
        IPAddress ipAddr = ipHost.AddressList[0];
        IPEndPoint endPoint = new IPEndPoint(ipAddr, 7777/* Port */);

        Socket socket = new Socket(endPoint.AddressFamily, 
                                    SocketType.Stream, 
                                    ProtocolType.Tcp);

        try
        {
            socket.Connect(endPoint);
            Console.WriteLine($"Connected To {socket.RemoteEndPoint.ToString()}...");

            // 보내는 부분
            byte[] sendBuff = Encoding.UTF8.GetBytes("Hello World!");
            int sendBytes = socket.Send(sendBuff);

            // 받는 부분
            byte[] recvBuff = new byte[1024];
            int recvBytes = socket.Receive(recvBuff);
            string recvData = Encoding.UTF8.GetString(recvBuff, 0, recvBytes);
            Console.WriteLine($"[From Server] {recvData}");

            // 나간다
            socket.Shutdown(SocketShutdown.Both);
            socket.Close();
        }
        catch (Exception e)
        {
            Console.WriteLine(e);
        }
    }
}
```