---
layout: default
title: "[구현] DEADLOCK 감지기"
parent: "(C++) Multi Thread"
grand_parent: "Game Server 👾"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get This Code 🌎](https://github.com/EasyCoding-7/Windows_Game_Server_Tutorial/tree/RA-Tag-04)

---

## 보통 DeadLock은 어떤상황에 나타날까?

```
----<A>----                     ----<B>----
|         |                     |         |
| Locked  |  ----- Lock ----->  | Locked  |
|         |  <---- Lock ------  |         |
-----------                     -----------
```

😺 A와 B 두개의 Thread가 있을때 각각을 Lock 잡은 이후 서로 상대방의 Lock을 기다릴때 Deadlock이 발생한다.<br>
😺 이 Deadlock을 graph적으로 표현하면 위 그림처럼 서로를 가리키는 모습이된다.<br>
😺 그럼 이런 사이클(서로를 가리키는)모양의 그래프가 발생하지 않으면 되겠군?<br>
😺 목표 : **각 Thread를 graph화 하고, 사이클모양의 그래프가 발견될시 Crash를 발생해보자.**<br>

---

## 실사용은 이렇게 하고자 한다

```cpp
// ...

void Lock::WriteLock(const char* name)
{
    // 디버그모드에서 락프로파일러가 설치된다.
#if _DEBUG
	GDeadLockProfiler->PushLock(name);
#endif

	// ...


void Lock::WriteUnlock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PopLock(name);
#endif

	// ...
```

---

## 구현해보자

```cpp
#pragma once
#include <stack>
#include <map>
#include <vector>

/*--------------------
	DeadLockProfiler
---------------------*/

class DeadLockProfiler
{
public:
	void PushLock(const char* name);
	void PopLock(const char* name);
	void CheckCycle();

private:
	void Dfs(int32 index);

private:
	unordered_map<const char*, int32>	_nameToId;
	unordered_map<int32, const char*>	_idToName;
	stack<int32>						_lockStack;
	map<int32, set<int32>>				_lockHistory;

	Mutex _lock;

private:
	vector<int32>	_discoveredOrder;       // 노드가 발견된 순서를 기록하는 배열
	int32			_discoveredCount = 0;   // 노드가 발견된 순서
	vector<bool>	_finished;              // Dfs(i)가 종료 되었는지 여부
	vector<int32>	_parent;
};
```

```cpp
#include "pch.h"
#include "DeadLockProfiler.h"

/*--------------------
	DeadLockProfiler
---------------------*/

/*

* 문제 상황은 이거다

A                  B
|                  |
| <-----Lock-----> |
|                  | 

* A, B 두개의 스레드가 서로 락을 잡고 상대방 락이 풀리길 기다리다 데드락 발생
* PushLock을 하면서 서로가 락을 잡고 기다리는(사이클) 현상이 발생하는지 확인하자

*/

void DeadLockProfiler::PushLock(const char* name)
{
	LockGuard guard(_lock);

	// 아이디를 찾거나 발급한다.
	int32 lockId = 0;

	// 현재 잡혀있는 락인지 우선 확인하고
	auto findIt = _nameToId.find(name);
	if (findIt == _nameToId.end())
	{
		// 없는 락이라면 id를 생성
			// lockId로 _nameToId의 size를 넣음을 확인 (0 부터 시작해서 추가됨)
		lockId = static_cast<int32>(_nameToId.size());
		_nameToId[name] = lockId;
		_idToName[lockId] = name;
	}
	else
	{
		// 있는 락이라면 id를 넣는다
		lockId = findIt->second;
	}

	// 잡고 있는 락이 있었다면
	if (_lockStack.empty() == false)
	{
		/*
            일단 여기 들어온 이상, 다른 스레드에서 
            lock 잡고 있는 상태에서
            내가 lock잡으려고 시도한 케이스
		*/

		// 기존에 발견되지 않은 케이스라면 데드락 여부 다시 확인한다.
		const int32 prevId = _lockStack.top();
		if (lockId != prevId)   // 내 스레드가 아니라면 데드락 확인
		{
            // 다른 쓰레드가 락을 잡고있네?
			set<int32>& history = _lockHistory[prevId]; // _lockHistory ->> prevId가 lockId를 잡고있음을 알린다.
			if (history.find(lockId) == history.end())
			{
                // prevId가 lockId에 lock에 걸릴수 있음을 의미
                /*

                    * 그래프로 표현하자면

                    [A] -> [B]
                    [B] -> [A]을 확인하면 된다.

                */

				// lockId(현재 thread)가 history를 기다리고 있음을 의미
				history.insert(lockId);
				CheckCycle();
			}
		}
	}

	// 현재 잡은락의 id를 stack에 쌓는다
	_lockStack.push(lockId);
}

void DeadLockProfiler::PopLock(const char* name)
{
	LockGuard guard(_lock);

	if (_lockStack.empty())
		CRASH("MULTIPLE_UNLOCK");

	int32 lockId = _nameToId[name];
	if (_lockStack.top() != lockId)
		CRASH("INVALID_UNLOCK");

	_lockStack.pop();
}

void DeadLockProfiler::CheckCycle()
{
	const int32 lockCount = static_cast<int32>(_nameToId.size());   // 현재 몇개의 스레드가 락을 잡고있는가
	_discoveredOrder = vector<int32>(lockCount, -1);    // 락 잡힌 갯수만큼 -1로 초기화
	_discoveredCount = 0;
	_finished = vector<bool>(lockCount, false);
	_parent = vector<int32>(lockCount, -1);

	for (int32 lockId = 0; lockId < lockCount; lockId++)
		Dfs(lockId);

	// 연산이 끝났으면 정리한다.
	_discoveredOrder.clear();
	_finished.clear();
	_parent.clear();
}

void DeadLockProfiler::Dfs(int32 here)
{
    // 현재 지점이 발견이 된 상태
	if (_discoveredOrder[here] != -1)
		return;

	_discoveredOrder[here] = _discoveredCount++;

	// 모든 인접한 정점을 순회한다.
	auto findIt = _lockHistory.find(here);  // here가 어떤 Thread를 lock잡고있는지 확인
	if (findIt == _lockHistory.end())
	{
        // 다른 정점을 잡고있는 애가 없음.
		_finished[here] = true;
		return;
	}

	set<int32>& nextSet = findIt->second;
	for (int32 there : nextSet)
	{
		// 아직 방문한 적이 없다면 방문한다.
		if (_discoveredOrder[there] == -1)
		{
			_parent[there] = here;
			Dfs(there);
			continue;
		}

		// here가 there보다 먼저 발견되었다면, there는 here의 후손이다. (순방향 간선)
		if (_discoveredOrder[here] < _discoveredOrder[there])
			continue;

		// 순방향이 아니고, Dfs(there)가 아직 종료하지 않았다면, there는 here의 선조이다. (역방향 간선)
		if (_finished[there] == false)
		{
			printf("%s -> %s\n", _idToName[here], _idToName[there]);

			int32 now = here;
			while (true)
			{
				printf("%s -> %s\n", _idToName[_parent[now]], _idToName[now]);
				now = _parent[now];
				if (now == there)
					break;
			}

			CRASH("DEADLOCK_DETECTED");
		}
        else
        {
            // 하나만 더 고려해 보자
            // if (_finished[there] == true)
            // 역방향 간선이 있었으나 _finished를 한 경우는 있을까?

            // 없음.
            // _finished[there] == true의 조건이
                // lock을 잡고있는 자녀가 없거나
                // 자녀의 dfs가 모두 끝난경우 밖에 없기에 무조건 잡힌다
        }
	}

	_finished[here] = true;
}
```