---
layout: default
title: "[구현] ThreadManager"
parent: "(C++ IOCP)"
grand_parent: "Game Server 👾"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get This Code 🌎](https://github.com/EasyCoding-7/Windows_Game_Server_Tutorial/tree/RA-Tag-02)

---

## 정리

🍒 `GThreadManager->Launch(callback);` 을 통해서 Thread를 간단하게 써보자.

---

## 구현

```cpp
// GameServer.cpp

// ...

// CoreGlobal에 ThreadManager가 포함되어있고
CoreGlobal Core;

void ThreadMain()
{
	while (true)
	{
		cout << "Hello ! I am thread... " << LThreadId << endl;
		this_thread::sleep_for(1s);
	}
}

int main()
{
	for (int32 i = 0; i < 5; i++)
	{
		// Thread에 function을 추가하면 된다.
		GThreadManager->Launch(ThreadMain);
	}

	GThreadManager->Join();
}
```

전역으로 관리하는게 사용하기 편하겠지?

```cpp
// CoreGlobal.cpp

// ...

ThreadManager* GThreadManager = nullptr;

CoreGlobal::CoreGlobal()
{
	GThreadManager = new ThreadManager();
}
```

```cpp
// ...

void ThreadManager::Launch(function<void(void)> callback)
{
	LockGuard guard(_lock);

	_threads.push_back(thread([=]()
		{
			InitTLS();
			callback();
			DestroyTLS();		// DestryoTLS는 미구현(아직은 필요없다)
		}));
}

void ThreadManager::Join()
{
	for (thread& t : _threads)
	{
		if (t.joinable())
            // 이거 가끔 헷갈림 join을 한다고 thread가 시작되는게 아님
            // join은 종료를 대기할 뿐...
			t.join();
	}
	_threads.clear();
}

void ThreadManager::InitTLS()
{
	static Atomic<uint32> SThreadId = 1;
	LThreadId = SThreadId.fetch_add(1);
	// LThreadId는
	// thread_local uint32 LThreadId = 0;
	// TLS로 선언되어 있고 ID값을 보관하게 된다.
}

// ...
```