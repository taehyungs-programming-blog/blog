---
layout: default
title: "18. JobQueue"
parent: "(basic)"
grand_parent: "(GameServer C# 🎯)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* 현재 코드의 문제점

```csharp
public void Broadcast(ClientSession session, string chat)
{
    S_Chat packet = new S_Chat();
    packet.playerId = session.SessionID;

    packet.chat = chat + $"I am {packet.playerId}";

    ArraySegment<byte> segment = packet.Write();

    // 여기서 for문을 돌며 병목현상이 발생.
    lock(_lock)
    {
        foreach(ClientSession sessions in _sessions)
        {
            sessions.Send(segment);
        }
    }
}
```

* **Command Pattern**으로 해결하고자 한다.
    * 명령이 들어오면 명령을 특정 Thread의 Queue에 넣은 후 명령의 처리는 특정 Thread가 하게 된다.
    * 핵심은 수신 thread와 **명령을 처리하는 thread를 나눈다는데** 있다.(수신 thread가 명령처리까지 하게되면 무한 대기에 걸리게 되기에.)
    * 추가적으로 수신된 명령을 취소, 수정도 가능하게 된다.

---

## JobQueue - 1

* [Get Code](https://github.com/Arthur880708/CSharp_GameServer_Basic_Examples/tree/Tag-38-jobqueue-1)

* 생각보다 간단하다 JobQueue를 만들어서 그 JobQueue가 모든 Job(Task)를 처리하게 하면된다.

```csharp
namespace ServerCore
{
    public interface IJobQueue
    {
        void Push(Action job);
    }

    public class JobQueue : IJobQueue
    {
        Queue<Action> _jobQueue = new Queue<Action>();
        object _lock = new object();
        bool _flush = false;

        public void Push(Action job)
        {
            bool flush = false;

            lock(_lock)
            {
                _jobQueue.Enqueue(job);
                if (_flush == false)
                    flush = _flush = true;
            }

            if (flush)
                Flush();
        }

        Action Pop()
        {
            lock (_lock)
            {
                if (_jobQueue.Count == 0)
                {
                    _flush = false;
                    return null;
                }
                return _jobQueue.Dequeue();
            }
        }

        void Flush()
        {
            while(true)
            {
                Action action = Pop();
                if (action == null) return;

                action.Invoke();
            }
        }
    }
}
```

```csharp
class GameRoom : IJobQueue
{
    List<ClientSession> _sessions = new List<ClientSession>();
    JobQueue _jobQueue = new JobQueue();

    public void Enter(ClientSession session)
    {
        _sessions.Add(session);
        session.Room = this;
    }

    public void Leave(ClientSession session)
    {
        _sessions.Remove(session);
    }

    public void Broadcast(ClientSession session, string chat)
    {
        S_Chat packet = new S_Chat();
        packet.playerId = session.SessionID;

        // 테스트를 위해 데이터를 약간 수정해 본다.
        packet.chat = chat + $"I am {packet.playerId}";

        ArraySegment<byte> segment = packet.Write();

        foreach(ClientSession sessions in _sessions)
        {
            sessions.Send(segment);
        }
    }

    public void Push(Action job)
    {
        _jobQueue.Push(job);
    }
}
```

---

## JobQueue - 2

* [Get Code](https://github.com/Arthur880708/CSharp_GameServer_Basic_Examples/tree/Tag-39-jobqueue-2)

* JobQueue를 만드는 방법이 하나 더 있다.

```csharp
namespace Server
{
    interface ITask
    {
        void Execute();
    }

    // class로 Task(Queue)를 만드는 방법
        // 뭐가 좋다고 할 순 없음. 본인이 마음에 드는 방식을 사용하시요.
        // 개인적으로 JobQueue를 쓰는 lambda Capture가 더 낫다고 생각.
    class BroadcastTask : ITask
    {
        GameRoom _room;
        ClientSession _session;
        string _chat;

        BroadcastTask(GameRoom room, ClientSession session, string chat)
        {
            _room = _room;
            _session = session;
            _chat = chat;
        }

        public void Execute()
        {
            _room.Broadcast(_session, _chat);

        }
    }

    public class TestQueue
    {
        Queue<ITask> _queue = new Queue<ITask>();
    }
}
```