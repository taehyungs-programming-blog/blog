---
layout: default
title: "16. RecvBuffer"
parent: "(C# IOCP)"
grand_parent: "Game Server 👾"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code](https://github.com/EasyCoding-7/CSharp_Windows_GameServer_Tutorial/tree/Tag-23-ReadBuffer)

```csharp
void RegisterRecv()
{
    _recvBuffer.Clean();
    ArraySegment<byte> segment = _recvBuffer.WriteSegment;
    _recvArgs.SetBuffer(segment.Array, segment.Offset, segment.Count);

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
            if(_recvBuffer.OnWrite(args.BytesTransferred) == false)
            {
                Disconnect();
                return;
            }

            int processLen = OnRecv(_recvBuffer.ReadSegment);
            if(processLen < 0 || _recvBuffer.DataSize < processLen)
            {
                Disconnect();
                return;
            }

            if(_recvBuffer.OnRead(processLen) == false)
            {
                Disconnect();
                return;
            }

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
```

```csharp
namespace ServerCore
{
    class RecvBuffer
    {
        ArraySegment<byte> _buffer;
        int _readPos;   // 데이터를 컨텐츠 단에서 얼마나 읽어 갔는지
        int _writePos;  // 데이터를 얼마나 받아 왔는지

        public RecvBuffer(int bufferSize)
        {
            _buffer = new ArraySegment<byte>(new byte[bufferSize], 0, bufferSize);
        }

        public int DataSize { get { return _writePos - _readPos; } }
        public int FreeSize { get { return _buffer.Count - _writePos; } }

        public ArraySegment<byte> ReadSegment
        {
            get { return new ArraySegment<byte>(_buffer.Array, _buffer.Offset + _readPos, DataSize); }
        }

        public ArraySegment<byte> WriteSegment
        {
            get { return new ArraySegment<byte>(_buffer.Array, _buffer.Offset + _writePos, FreeSize); }
        }

        public void Clean()
        {
            int dataSize = DataSize;
            if(dataSize == 0)
            {
                // 남은 데이터없을시 커서위치만 변경
                _readPos = _writePos = 0;
            }
            else
            {
                Array.Copy(_buffer.Array, 
                            _buffer.Offset + _readPos, 
                            _buffer.Array, 
                            _buffer.Offset, 
                            dataSize);
                _readPos = 0;
                _writePos = dataSize;
            }
        }

        public bool OnRead(int numOfBytes)
        {
            if (numOfBytes > DataSize)
                return false;

            _readPos += numOfBytes;
            return true;
        }

        public bool OnWrite(int numOfBytes)
        {
            if (numOfBytes > FreeSize)
                return false;

            _writePos += numOfBytes;
            return true;
        }
    }
}
```