---
layout: default
title: "11. Socket - 4"
parent: "(C# IOCP)"
grand_parent: "Game Server 👾"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code](https://github.com/EasyCoding-7/CSharp_Windows_GameServer_Tutorial/tree/Tag-18-Session-2)

🍁 기존에 Send 데이터가 많을경우 보내지못하는 현상발생 queue에 데이터를 쌓고 보내게 수정

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
