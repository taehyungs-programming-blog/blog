---
layout: default
title: "[구현] JobQueue - 4(thread를 전환하며 JobQueue Flush)"
parent: "(C++) 상세 구현"
grand_parent: "Game Server 👾"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get This Code 🌎](https://github.com/EasyCoding-7/Windows_Game_Server_Tutorial/tree/RA-Tag-33)

---

🐱‍👤 아직 모든 문제가 해결된 것은 아니다

```cpp
// Job을 Flush할때 메인 Thread에서 하고있는데
    // 여러 Thread 혹은 여러 객체에서 별도로 Job을 처리해야 할때가 있다.
while (true)
{
    GRoom->FlushJob();
    this_thread::sleep_for(1ms);
}
```

---

🐱‍👤 구현은 쉽기에 별도의 설명은 생략

```cpp
void JobQueue::Push(JobRef&& job)
{
	const int32 prevCount = _jobCount.fetch_add(1);
	_jobs.Push(job); // WRITE_LOCK

	// 첫번째 Job을 넣은 쓰레드가 실행까지 담당
	if (prevCount == 0)
	{
		Execute();
	}
}

// 문제점
    // 1) 일감이 너~무 몰리면?
    // 2) DoAsync 타고 타고 가서~ 절대 끝나지 않는 상황 (일감이 한 쓰레드한테 몰림)
void JobQueue::Execute()
{
	while (true)
	{
		Vector<JobRef> jobs;
		_jobs.PopAll(OUT jobs);

		const int32 jobCount = static_cast<int32>(jobs.size());
		for (int32 i = 0; i < jobCount; i++)
			jobs[i]->Execute();

		// 남은 일감이 0개라면 종료
		if (_jobCount.fetch_sub(jobCount) == jobCount)
		{
			return;
		}
	}
}
```

```cpp
bool Handle_C_CHAT(PacketSessionRef& session, Protocol::C_CHAT& pkt)
{
	std::cout << pkt.msg() << endl;

	Protocol::S_CHAT chatPkt;
	chatPkt.set_msg(pkt.msg());
	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(chatPkt);

	GRoom->DoAsync(&Room::Broadcast, sendBuffer);

	return true;
}
```

---

🐱‍👤 하나의 Thread에 일이 너무 몰릴수 있는데 이 문제를 해결해 보자.<br>
🐱‍👤 사실상 구현은 간단하다. 이미 Execute 중이면 Job을 넣기만 하고 Execute중이 아니면 해당 Thread가 실행

```cpp
void JobQueue::Push(JobRef&& job)
{
	const int32 prevCount = _jobCount.fetch_add(1);
	_jobs.Push(job); // WRITE_LOCK

	// 첫번째 Job을 넣은 쓰레드가 실행까지 담당
	if (prevCount == 0)
	{
		// 이미 실행중인 JobQueue가 없으면 실행
		if (LCurrentJobQueue == nullptr)
		{
			Execute();
		}
		else
		{
			// 여유 있는 다른 쓰레드가 실행하도록 GlobalQueue에 넘긴다
			GGlobalQueue->Push(shared_from_this());
		}
	}
}

// 1) 일감이 너~무 몰리면?
void JobQueue::Execute()
{
	LCurrentJobQueue = this;

	while (true)
	{
		Vector<JobRef> jobs;
		_jobs.PopAll(OUT jobs);

		const int32 jobCount = static_cast<int32>(jobs.size());
		for (int32 i = 0; i < jobCount; i++)
			jobs[i]->Execute();

		// 남은 일감이 0개라면 종료
		if (_jobCount.fetch_sub(jobCount) == jobCount)
		{
			LCurrentJobQueue = nullptr;
			return;
		}

		const uint64 now = ::GetTickCount64();
		if (now >= LEndTickCount)
		{
			LCurrentJobQueue = nullptr;
			// 여유 있는 다른 쓰레드가 실행하도록 GlobalQueue에 넘긴다
			GGlobalQueue->Push(shared_from_this());
			break;
		}			
	}
}
```

```cpp
// Queue의 실행은

void DoWorkerJob(ServerServiceRef& service)
{
	while (true)
	{
		LEndTickCount = ::GetTickCount64() + WORKER_TICK;

		// 네트워크 입출력 처리 -> 인게임 로직까지 (패킷 핸들러에 의해)
		service->GetIocpCore()->Dispatch(10);

		// 글로벌 큐
		ThreadManager::DoGlobalQueueWork();
        /*
            void ThreadManager::DoGlobalQueueWork()
            {
                while (true)
                {
                    uint64 now = ::GetTickCount64();
                    if (now > LEndTickCount)
                        break;

                    JobQueueRef jobQueue = GGlobalQueue->Pop();
                    if (jobQueue == nullptr)
                        break;

                    jobQueue->Execute();
                }
            }
        */
	}
}
```