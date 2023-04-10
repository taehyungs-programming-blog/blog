---
layout: default
title: "12. SendBuffer"
parent: "(기초)"
grand_parent: "(GameServer C# 🎯)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code](https://github.com/Arthur880708/CSharp_GameServer_Basic_Examples/tree/Tag-24-SendBuffer)

* 우선 SendBuffer를 외부에서 관리하는 이유는 A라는 캐릭이 주변 100명에게 스킬을 쓴다고 가정해 보자.
* SendBuffer를 Session 내부적으로 관리할 시 Send(100회)를 할때마다 SendBuffer를 작성해야한다.(비효율)

```csharp
namespace ServerCore
{
    public class SendBufferHelper
    {
        public static ThreadLocal<SendBuffer> CurrentBuffer = new ThreadLocal<SendBuffer>(() => { return null; });

        public static int ChunkSize { get; set; } = 4096 * 100;

        public static ArraySegment<byte> Open(int reserveSize)
        {
            if (CurrentBuffer.Value == null)
                CurrentBuffer.Value = new SendBuffer(ChunkSize);

            if(CurrentBuffer.Value.FreeSize < reserveSize)
                CurrentBuffer.Value = new SendBuffer(ChunkSize);

            return CurrentBuffer.Value.Open(reserveSize);
        }

        public static ArraySegment<byte> Close(int usedSize)
        {
            return CurrentBuffer.Value.Close(usedSize);
        }
    }

    public class SendBuffer
    {
        byte[] _buffer;
        int _usedSize = 0;

        public int FreeSize { get { return _buffer.Length - _usedSize; } }

        public SendBuffer(int chunkSize)
        {
            _buffer = new byte[chunkSize];
        }

        public ArraySegment<byte> Open(int reserveSize)
        {
            if (reserveSize > FreeSize)
                return new ArraySegment<byte>();

            return new ArraySegment<byte>(_buffer, _usedSize, reserveSize);
        }

        public ArraySegment<byte> Close(int usedSize)
        {
            ArraySegment<byte> segment = new ArraySegment<byte>(_buffer, _usedSize, usedSize);
            _usedSize += usedSize;
            // 여기서 재활용할것도 아닌데 버퍼를 왜 이렇게 쓰지? 할수 있는데
            // 미리 버퍼를 크게잡아놨다가 잘라가며 쓰는게 한번에 버퍼를 할당후 사용할수 있어 더 효율적이다.

            return segment;
        }
    }
}
```

```csharp
// Server
// Program.cs

class Knight
{
    public int hp;
    public int attack;
}

class GameSession : Session
{
    public override void OnConnected(EndPoint endPoint)
    {
        Console.WriteLine($"[OnConnected] {endPoint}");

        Knight kinght = new Knight() { hp = 100, attack = 10 };

        // 보내는 부분
        ArraySegment<byte> openSegment = SendBufferHelper.Open(4096);
        byte[] buffer = BitConverter.GetBytes(kinght.hp);
        byte[] buffer2 = BitConverter.GetBytes(kinght.attack);
        Array.Copy(buffer, 0, openSegment.Array, openSegment.Offset, buffer.Length);
        Array.Copy(buffer2, 0, openSegment.Array, openSegment.Offset + buffer.Length, buffer2.Length);
        ArraySegment<byte> sendBuffer = SendBufferHelper.Close(buffer.Length + buffer2.Length);

        Send(sendBuffer);
        Thread.Sleep(1000);
        Disconnect();
    }
```