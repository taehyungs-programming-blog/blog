---
layout: default
title: "20. JobTimer"
parent: "(기초)"
grand_parent: "(GameServer C# 🎯)"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code](https://github.com/Arthur880708/CSharp_GameServer_Basic_Examples/tree/Tag-41-jobtimer)

```csharp
// ...

static void Main(string[] args)
{
    // ...

    while (true)
    {
        // 현재 main Thread에서 이런식으로 Flush해주는데
        // 너무 대중없다. 예를들어 Room이 여러개일경우 어떻게 할것인가?
        Room.Push(() => Room.Flush());
        Thread.Sleep(250);
    }
}
```

---

구현

```csharp
namespace Server
{
	struct JobTimerElem : IComparable<JobTimerElem>
	{
		public int execTick; // 실행 시간
		public Action action;

		public int CompareTo(JobTimerElem other)
		{
			return other.execTick - execTick;
		}
	}

	class JobTimer
	{
        // 우선순위 큐에 넣는이유는 실행시간이 가장 적게남은 Job을꺼내기 위해서이다.
		PriorityQueue<JobTimerElem> _pq = new PriorityQueue<JobTimerElem>();
		object _lock = new object();

		public static JobTimer Instance { get; } = new JobTimer();

		public void Push(Action action, int tickAfter = 0)
		{
			JobTimerElem job;
			job.execTick = System.Environment.TickCount + tickAfter;
			job.action = action;

			lock (_lock)
			{
				_pq.Push(job);
			}
		}

		public void Flush()
		{
			while (true)
			{
				int now = System.Environment.TickCount;

				JobTimerElem job;

				lock (_lock)
				{
					if (_pq.Count == 0)
						break;

					job = _pq.Peek();
					if (job.execTick > now)
						break;

					_pq.Pop();
				}

				job.action.Invoke();
			}
		}
	}
}

```