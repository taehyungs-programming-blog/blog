---
layout: default
title: "08. Socket"
parent: "(기초)"
grand_parent: "(GameServer C# 🎯)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code](https://github.com/Arthur880708/CSharp_GameServer_Basic_Examples/tree/Tag-15-SocketProgramming)

## Step. 1 - 간단한 Echo Server, Client

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

---

## Step. 2 - Server의 Function을 분리해 보자.

* [Get Code](https://github.com/Arthur880708/CSharp_GameServer_Basic_Examples/tree/Tag-16-ListenerClass)


* 우선 DummyClient를 건드리지 않고 Server에 몇가지 개선을 해볼 예정
* Listner class를 통해 Accept를 동작을 별도로 분리할 예정

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

---

## Step. 3 - Session을 통해 Recv, Send를 받아보자.

* [Get Code](https://github.com/Arthur880708/CSharp_GameServer_Basic_Examples/tree/Tag-17-Session1)

* Session을 통해 Socket의 Start, Send Disconnect를 구현할예정
* Session을 별도로 나누는 이유? -> Recv, Send를 Session을 통해할 예정

```csharp
namespace ServerCore
{
    class Program
    {
        static Listener _listener = new Listener();

        static void OnAcceptHandler(Socket clientSocket)
        {
            try
            {
                Session session = new Session();

                /* Session의 아래 기능을 구현 예정 */

                // 1. Start
                session.Start(clientSocket);

                byte[] sendBuff = Encoding.UTF8.GetBytes("Welcom to MMORPG Server ");

                // 2. Send
                session.Send(sendBuff);

                Thread.Sleep(1000);

                // 3. Disconnect
                session.Disconnect();
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
            }
        }

        static void Main(string[] args)
        {
            string host = Dns.GetHostName();
            IPHostEntry ipHost = Dns.GetHostEntry(host);
            IPAddress ipAddr = ipHost.AddressList[0];
            IPEndPoint endPoint = new IPEndPoint(ipAddr, 7777);
            _listener.Init(endPoint, OnAcceptHandler);

            while (true)
            {
            }
        }
    }
}
```

```csharp
namespace ServerCore
{
    class Session
    {
        Socket _socket;
        int _disconnected = 0;

        public void Start(Socket socket)
        {
            _socket = socket;
            SocketAsyncEventArgs recvArgs = new SocketAsyncEventArgs();
            recvArgs.Completed += new EventHandler<SocketAsyncEventArgs>(OnRecvComplete);
            recvArgs.SetBuffer(new byte[1024], 0/*시작인덱스*/, 1024);

            RegisterRecv(recvArgs);
        }

        public void Send(byte[] sendBuff)
        {
            _socket.Send(sendBuff);
        }

        public void Disconnect()
        {
            if (Interlocked.Exchange(ref _disconnected, 1) == 1)
                return;

            _socket.Shutdown(SocketShutdown.Both);
            _socket.Close();
        }

        void RegisterRecv(SocketAsyncEventArgs args)
        {
            bool pending = _socket.ReceiveAsync(args);
            if (pending == false)
                OnRecvComplete(null, args);
        }

        void OnRecvComplete(object sender, SocketAsyncEventArgs args)
        {
            if (args.BytesTransferred > 0 && args.SocketError == SocketError.Success)
            {
                try
                {
                    string recvData = Encoding.UTF8.GetString(args.Buffer, args.Offset, args.BytesTransferred);
                    Console.WriteLine($"[From Client] {recvData}");
                    RegisterRecv(args);
                }
                catch (Exception e)
                {
                    Console.WriteLine(e);
                }
            }
            else
            {
                // Disconnect
            }
        }
    }
}
```

---

## Step. 4 - Send Queue를 만들어 보자.

* [Get Code](https://github.com/Arthur880708/CSharp_GameServer_Basic_Examples/tree/Tag-18-Session-2)

* 기존에 Send 데이터가 많을경우 보내지못하는 현상발생 queue에 데이터를 쌓고 보내게 수정

```csharp
namespace ServerCore
{
    class Session
    {
        Socket _socket;
        int _disconnected = 0;

        // Queue에 보낼 데이터를 미리 담아두자
        Queue<byte[]> _sendQueue = new Queue<byte[]>();

        // pending 정보도 전역으로 관리하자
        bool _pending = false;

        // SocketAsyncEventArgs를 재사용하자
        SocketAsyncEventArgs _sendArgs = new SocketAsyncEventArgs();

        // lock도 사용될 예정
        object _lock = new object();


        public void Start(Socket socket)
        {
            _socket = socket;
            SocketAsyncEventArgs recvArgs = new SocketAsyncEventArgs();
            recvArgs.Completed += new EventHandler<SocketAsyncEventArgs>(OnRecvComplete);
            recvArgs.SetBuffer(new byte[1024], 0/*시작인덱스*/, 1024);

            // Send 델리게이터를 시작시점에 연결
            _sendArgs.Completed += new EventHandler<SocketAsyncEventArgs>(OnSendCompleted);

            RegisterRecv(recvArgs);
        }

        public void Send(byte[] sendBuff)
        {
            lock (_lock)        // 공통데이터 queue를 사용해야 하기에 lock을 잡는다
            {
                _sendQueue.Enqueue(sendBuff);
                if (_pending == false)
                    RegisterSend();
            }
        }

        void RegisterSend()
        {
            // 여기서도 queue를 사용하지만 lock을 안잡는 이유는 무조건 lock이 잡힌 이후에 여기로 들어오기 때문
            _pending = true;
            byte[] buff = _sendQueue.Dequeue();
            _sendArgs.SetBuffer(buff, 0, buff.Length);

            bool pending = _socket.SendAsync(_sendArgs);
            OnSendCompleted(null, _sendArgs);
        }

        void OnSendCompleted(object sender, SocketAsyncEventArgs args)
        {
            lock (_lock)        // 공통데이터 queue를 사용해야 하기에 lock을 잡는다
            {
                if (args.BytesTransferred > 0 && args.SocketError == SocketError.Success)
                {
                    try
                    {
                        // SendQueue안에 남아있는지 확인
                        if (_sendQueue.Count > 0)
                        {
                            RegisterSend();
                        }
                        else
                            _pending = false;
                    }
                    catch (Exception e)
                    {

                    }
                }
                else
                {
                    Disconnect();
                }
            }
        }

        public void Disconnect()
        {
            if (Interlocked.Exchange(ref _disconnected, 1) == 1)
                return;

            _socket.Shutdown(SocketShutdown.Both);
            _socket.Close();
        }

        void RegisterRecv(SocketAsyncEventArgs args)
        {
            bool pending = _socket.ReceiveAsync(args);
            if (pending == false)
                OnRecvComplete(null, args);
        }

        void OnRecvComplete(object sender, SocketAsyncEventArgs args)
        {
            if (args.BytesTransferred > 0 && args.SocketError == SocketError.Success)
            {
                try
                {
                    string recvData = Encoding.UTF8.GetString(args.Buffer, args.Offset, args.BytesTransferred);
                    Console.WriteLine($"[From Client] {recvData}");
                    RegisterRecv(args);
                }
                catch (Exception e)
                {
                    Console.WriteLine(e);
                }
            }
            else
            {
                // Disconnect
            }
        }
    }
}
```

---

## Step. 5 - Send를 모았다가 보내게 수정

* [Get Code](https://github.com/Arthur880708/CSharp_GameServer_Basic_Examples/tree/Tag-19-Session-3)

* `SocketAsyncEventArgs`의 `SetBuffer`말고 `BufferList`이용해 한번에 보내보자.

```csharp
namespace ServerCore
{
    class Session
    {
        Socket _socket;
        int _disconnected = 0;

        // Queue에 보낼 데이터를 미리 담아두자
        Queue<byte[]> _sendQueue = new Queue<byte[]>();

        // SocketAsyncEventArgs를 재사용하자
        SocketAsyncEventArgs _sendArgs = new SocketAsyncEventArgs();
        SocketAsyncEventArgs _recvArgs = new SocketAsyncEventArgs();

        // lock도 사용될 예정
        object _lock = new object();

        // 버퍼를 리스트로 관리해서 한 번에 보내자
        List<ArraySegment<byte>> _pendingList = new List<ArraySegment<byte>>();

        public void Start(Socket socket)
        {
            _socket = socket;
            _recvArgs.Completed += new EventHandler<SocketAsyncEventArgs>(OnRecvComplete);
            _recvArgs.SetBuffer(new byte[1024], 0/*시작인덱스*/, 1024);

            // Send 델리게이터를 시작시점에 연결
            _sendArgs.Completed += new EventHandler<SocketAsyncEventArgs>(OnSendCompleted);

            RegisterRecv();
        }

        public void Send(byte[] sendBuff)
        {
            lock (_lock)        // 공통데이터 queue를 사용해야 하기에 lock을 잡는다
            {
                _sendQueue.Enqueue(sendBuff);
                if (_pendingList.Count == 0)
                    RegisterSend();
            }
        }

        void RegisterSend()
        {
            while (_sendQueue.Count > 0)
            {
                byte[] buff = _sendQueue.Dequeue();
                _pendingList.Add(new ArraySegment<byte>(buff, 0, buff.Length));
            }

            _sendArgs.BufferList = _pendingList;

            bool pending = _socket.SendAsync(_sendArgs);
            if (pending == false)
                OnSendCompleted(null, _sendArgs);
        }

        void OnSendCompleted(object sender, SocketAsyncEventArgs args)
        {
            lock (_lock)        // 공통데이터 queue를 사용해야 하기에 lock을 잡는다
            {
                if (args.BytesTransferred > 0 && args.SocketError == SocketError.Success)
                {
                    try
                    {
                        _sendArgs.BufferList = null;
                        _pendingList.Clear();

                        // SendQueue안에 남아있는지 확인
                        if (_sendQueue.Count > 0)
                        {
                            RegisterSend();
                        }
                    }
                    catch (Exception e)
                    {

                    }
                }
                else
                {
                    Disconnect();
                }
            }
        }

        public void Disconnect()
        {
            if (Interlocked.Exchange(ref _disconnected, 1) == 1)
                return;

            _socket.Shutdown(SocketShutdown.Both);
            _socket.Close();
        }

        void RegisterRecv()
        {
            bool pending = _socket.ReceiveAsync(_recvArgs);
            if (pending == false)
                OnRecvComplete(null, _recvArgs);
        }

        void OnRecvComplete(object sender, SocketAsyncEventArgs args)
        {
            if (args.BytesTransferred > 0 && args.SocketError == SocketError.Success)
            {
                try
                {
                    string recvData = Encoding.UTF8.GetString(args.Buffer, args.Offset, args.BytesTransferred);
                    Console.WriteLine($"[From Client] {recvData}");
                    RegisterRecv();
                }
                catch (Exception e)
                {
                    Console.WriteLine(e);
                }
            }
            else
            {
                // Disconnect
            }
        }
    }
}
```

---

## Step. 6 - 

* [Get Code](https://github.com/Arthur880708/CSharp_GameServer_Basic_Examples/tree/Tag-20-Session-4)

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