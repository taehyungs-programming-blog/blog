---
layout: default
title: "[구현] Job Timer"
parent: "(C++) 상세 구현"
grand_parent: "Game Server 👾"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get This Code 🌎](https://github.com/EasyCoding-7/Windows_Game_Server_Tutorial/tree/RA-Tag-34)

---

🐳 **들어가기에 앞서** 느꼈을지 모르겠지만 현재 코드에 메모리 릭이있다.<br>
🐳 사실 smart pointer를 쓰다보면 어쩔수 없는 부분이 있는데 crash를 방지하는 대신 메모리릭이 발생하기도 한다.(crash보단 릭이 그래도 낫다;;)<br>
🐳 사실상 코드상의 모든 릭을 잡는 방법은 없고, 그래도 해결책이라고 한다면 소멸자에 로그를 찍는 것 AND 할당되는 메모리를 늘렸다가 줄였다가 해보는 Unit Test정도이다. 대부분 순환참조의 문제일태니 잘 해결하도록 하자...

---

```cpp
void DoWorkerJob(ServerServiceRef& service)
{
	while (true)
	{
		LEndTickCount = ::GetTickCount64() + WORKER_TICK;

		// 네트워크 입출력 처리 -> 인게임 로직까지 (패킷 핸들러에 의해)
		service->GetIocpCore()->Dispatch(10);

		// 예약된 일감 처리
		ThreadManager::DistributeReservedJobs();

		// 글로벌 큐
		ThreadManager::DoGlobalQueueWork();
	}
}
```

```cpp
void ThreadManager::DistributeReservedJobs()
{
    const uint64 now = ::GetTickCount64();

    GJobTimer->Distribute(now);
}
```

---

```cpp
void JobTimer::Reserve(uint64 tickAfter, weak_ptr<JobQueue> owner, JobRef job)
{
	const uint64 executeTick = ::GetTickCount64() + tickAfter;
	JobData* jobData = ObjectPool<JobData>::Pop(owner, job);

	WRITE_LOCK;

	_items.push(TimerItem{ executeTick, jobData });
}

void JobTimer::Distribute(uint64 now)
{
	// 한 번에 1 쓰레드만 통과
	if (_distributing.exchange(true) == true)
		return;

	Vector<TimerItem> items;

	{
		WRITE_LOCK;

		while (_items.empty() == false)
		{
			const TimerItem& timerItem = _items.top();
			if (now < timerItem.executeTick)
				break;

			items.push_back(timerItem);
			_items.pop();
		}
	}

	for (TimerItem& item : items)
	{
		if (JobQueueRef owner = item.jobData->owner.lock())
			owner->Push(item.jobData->job);

		ObjectPool<JobData>::Push(item.jobData);
	}

	// 끝났으면 풀어준다
	_distributing.store(false);
}
```