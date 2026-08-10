---
layout: default
title: "15. READ, WRITE LOCK"
parent: "(C++) Multi Thread"
grand_parent: "Game Server 👾"
great_grand_parent: "Legacy Archive"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get This Code 🌎](https://github.com/EasyCoding-7/Windows_Game_Server_Tutorial/tree/RA-Tag-03)

---

## 대략 이렇게 쓰고싶다

```cpp
class TestLock
{
    // LOCK을 사용하겠습니다.
	USE_LOCK;

public:
	int32 TestRead()
	{
		// Lock을 걸어줄 부분에 선언
		READ_LOCK;

		if (_queue.empty())
			return -1;

		return _queue.front();
	}

	void TestPush()
	{
		WRITE_LOCK;

		_queue.push(rand() % 100);
	}

//...
```

```cpp
// 매크로는 이렇게 선언됨.
// ...

#define USE_MANY_LOCKS(count)	Lock _locks[count];
#define USE_LOCK				USE_MANY_LOCKS(1)
#define	READ_LOCK_IDX(idx)		ReadLockGuard readLockGuard_##idx(_locks[idx]);
#define READ_LOCK				READ_LOCK_IDX(0)
#define	WRITE_LOCK_IDX(idx)		WriteLockGuard writeLockGuard_##idx(_locks[idx]);
#define WRITE_LOCK				WRITE_LOCK_IDX(0)

// ...
```

---

## 구현해보자

```cpp
/*--------------------------------------------
[WWWWWWWW][WWWWWWWW][RRRRRRRR][RRRRRRRR]
// 32bits를 사용하며 상위 16bits는 Write / 하위 16bits는 Read용 flag
W : WriteFlag (Exclusive Lock Owner ThreadId)
R : ReadFlag (Shared Lock Count)
---------------------------------------------*/

class Lock
{
    enum : uint32
    {
        ACQUIRE_TIMEOUT_TICK = 10000,       // 이 시간 이후 시도시 Crash
        MAX_SPIN_COUNT = 5000,              // 몇번을 자원 경합할 것인가
        WRITE_THREAD_MASK = 0xFFFF'0000,    // 상위 16bits Mask
        READ_COUNT_MASK = 0x0000'FFFF,      // 하위 16bits Mask
        EMPTY_FLAG = 0x0000'0000
    };

public:
    void WriteLock();
    void WriteUnlock();
    void ReadLock();
    void ReadUnlock();

private:
    Atomic<uint32> _lockFlag = EMPTY_FLAG;
    uint16 _writeCount = 0;
};
```

```cpp
#include "pch.h"
#include "Lock.h"
#include "CoreTLS.h"

void Lock::WriteLock()
{
	// 동일한 쓰레드가 소유하고 있다면 무조건 성공.
        // lock이 재귀적으로 호출되는 경우를 대비함.
		// 이미 lock이 잡혀있기에 return하는 것임.
	const uint32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadId == lockThreadId)
	{
		_writeCount++;
		return;
	}

	// 아무도 소유 및 공유하고 있지 않을 때, 경합해서 소유권을 얻는다.
	const int64 beginTick = ::GetTickCount64();
	const uint32 desired = ((LThreadId << 16) & WRITE_THREAD_MASK);
	while (true)
	{
        // MAX_SPIN_COUNT만큼 돌면서 자원 경합
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			uint32 expected = EMPTY_FLAG;

            // 현재 _lockFlag를 차지하고 있는 ThreadId가 없다면 내 ThreadId를 넣고 Lock
                // OUT은 #define OUT 개발자에게 expected값이 함수에 의해 변경될수 있다고 알려줌.
                // 추가적으로 누군가 Read중일때는 Write할수 없음을 기억.
				/*
					반대로 말하면 Write중일때는 Read가 가능하다는 말인데
					이러면 위험하다 사실
					그래서 대부분 WRITE_LOCK만 사용됨
					Write중일때 READ가 가능하다는 것을 보여주기위한 예시정도라 받아들이자
				*/
			if (_lockFlag.compare_exchange_strong(OUT expected, desired))
			{
				_writeCount++;
				return;
			}
		}

		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("LOCK_TIMEOUT");  // ACQUIRE_TIMEOUT_TICK 시간이상 대기시 Deadlock이라 생각한다.

        // thread를 양보
		this_thread::yield();
	}
}

void Lock::WriteUnlock()
{
	// ReadLock 다 풀기 전에는 WriteUnlock 불가능.
	if ((_lockFlag.load() & READ_COUNT_MASK) != 0)
		CRASH("INVALID_UNLOCK_ORDER");

	const int32 lockCount = --_writeCount;
	if (lockCount == 0)
		_lockFlag.store(EMPTY_FLAG);
}

void Lock::ReadLock()
{
	// 동일한 쓰레드가 소유하고 있다면 무조건 성공.
	const uint32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadId == lockThreadId)
	{
		_lockFlag.fetch_add(1);
		return;
	}

	// 아무도 소유하고 있지 않을 때 경합해서 공유 카운트를 올린다.
	const int64 beginTick = ::GetTickCount64();
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			uint32 expected = (_lockFlag.load() & READ_COUNT_MASK);
			if (_lockFlag.compare_exchange_strong(OUT expected, expected + 1))
				return;
		}

		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("LOCK_TIMEOUT");

		this_thread::yield();
	}
}

void Lock::ReadUnlock()
{
    // fetch_sub(1)은 -1하기 전의 값이 리턴되는데 -1 전의 값이 0이면 뭔가 이상한거... -> Crash!!
	if ((_lockFlag.fetch_sub(1) & READ_COUNT_MASK) == 0)
		CRASH("MULTIPLE_UNLOCK");
}
```