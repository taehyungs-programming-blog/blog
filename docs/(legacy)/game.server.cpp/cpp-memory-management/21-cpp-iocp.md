---
layout: default
title: "[구현] Memory pool - 1"
parent: "(C++) Memory Management"
grand_parent: "Game Server 👾"
great_grand_parent: "Legacy Archive"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get This Code 🌎](https://github.com/EasyCoding-7/Windows_Game_Server_Tutorial/tree/RA-Tag-09)

---

## 하고자 하는 것.

* 이제 파편화, 재사용 문제를 해결해 보자
    * `Memory Pool` : 메모리를 바로 해지하지말고 임시로 보관했다가 필요하면 꺼내쓰겠다
    * 👉 재 할당 시간을 아껴보자, 메모리파편화 방지도 할 수 있을 듯
    * 참고로 아래 내용을 보다보면 `MemoryHeader`를 굳이 왜쓰는지 싶은데 이후에 Header에 필요한 부분이 추가되기에 이번 강좌에서는 받아들이자.

---

## 실사용

```cpp
int main()
{
	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([]()
			{
				while (true)
				{
					Vector<Knight> v(10);

                    // 메모리 할당시 구현한 Allocator를 이용해 할당하는데
					Map<int32, Knight> m;
					m[100] = Knight();

					this_thread::sleep_for(10ms);
				}
			});
	}

	GThreadManager->Join();
}
```

```cpp
// 메모리풀 Allocator를 사용하게 강제할 예정이다.

void* Memory::Allocate(int32 size)
{
	MemoryHeader* header = nullptr;
	const int32 allocSize = size + sizeof(MemoryHeader);

	if (allocSize > MAX_ALLOC_SIZE)
	{
		// 메모리 풀링 최대 크기를 벗어나면 일반 할당
		header = reinterpret_cast<MemoryHeader*>(::malloc(allocSize));
	}
	else
	{
		// 메모리 풀에서 꺼내온다
		header = _poolTable[allocSize]->Pop();
	}

	return MemoryHeader::AttachHeader(header, allocSize);
}
```

---

## 구현

```cpp
#pragma once

/*-----------------
    MemoryHeader
------------------*/

struct MemoryHeader
{
    // [MemoryHeader][Data]
    MemoryHeader(int32 size) : allocSize(size) { }

    static void* AttachHeader(MemoryHeader* header, int32 size)
    {
        new(header)MemoryHeader(size); // placement new
        // header 메모리할당은 되어있으니 생성자만 호출해줘
        return reinterpret_cast<void*>(++header);
    }

    static MemoryHeader* DetachHeader(void* ptr)
    {
        MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1;
        return header;
    }

    // 아직은 헤더에 Size말고 별도의 정보는 없다.
    int32 allocSize;

    // TODO : 필요한 추가 정보
};

/*-----------------
    MemoryPool
------------------*/

class MemoryPool
{
public:
    MemoryPool(int32 allocSize);
    ~MemoryPool();

    void            Push(MemoryHeader* ptr);
    MemoryHeader*   Pop();

private:
    int32 _allocSize = 0;
    atomic<int32> _allocCount = 0;

    USE_LOCK;
    queue<MemoryHeader*> _queue;
};
```

```cpp
#include "pch.h"
#include "MemoryPool.h"

/*-----------------
    MemoryPool
------------------*/

MemoryPool::MemoryPool(int32 allocSize) : _allocSize(allocSize)
{
}

MemoryPool::~MemoryPool()
{
    while (_queue.empty() == false)
    {
        MemoryHeader* header = _queue.front();
        _queue.pop();
        ::free(header);
    }
}

void MemoryPool::Push(MemoryHeader* ptr)
{
    WRITE_LOCK;
    ptr->allocSize = 0;
    // Pool에 메모리 반납
    _queue.push(ptr);
    _allocCount.fetch_sub(1);
}

MemoryHeader* MemoryPool::Pop()
{
    MemoryHeader* header = nullptr;
    {
        WRITE_LOCK;
        // Pool에 여분이 있는지?
        if (_queue.empty() == false)
        {
            // 있으면 하나 꺼내온다
            header = _queue.front();
            _queue.pop();
        }
    }

    // 없으면 새로 만들다
    if (header == nullptr)
    {
        header = reinterpret_cast<MemoryHeader*>(::malloc(_allocSize));
    }
    else
    {
        ASSERT_CRASH(header->allocSize == 0);
    }

    _allocCount.fetch_add(1);
    return header;
}
```

```cpp
#pragma once
#include "Allocator.h"

class MemoryPool;

/*-------------
    Memory
---------------*/

class Memory
{
    enum
    {
        // ~1024까지 32단위, ~2048까지 128단위, ~4096까지 256단위
        POOL_COUNT = (1024 / 32) + (1024 / 128) + (2048 / 256),
        MAX_ALLOC_SIZE = 4096
    };

public:
    Memory();
    ~Memory();

    void*   Allocate(int32 size);
    void    Release(void* ptr);

private:
    vector<MemoryPool*> _pools;
    // 메모리 크기 <-> 메모리 풀
    // O(1) 빠르게 찾기 위한 테이블
    MemoryPool* _poolTable[MAX_ALLOC_SIZE + 1];
};

template<typename Type, typename... Args>
Type* xnew(Args&&... args)
{
    Type* memory = static_cast<Type*>(xalloc(sizeof(Type)));
    new(memory)Type(forward<Args>(args)...); // placement new
    return memory;
}

template<typename Type>
void xdelete(Type* obj)
{
    obj->~Type();
    xrelease(obj);
}
```

```cpp
#include "pch.h"
#include "Memory.h"
#include "MemoryPool.h"

/*-------------
    Memory
---------------*/

Memory::Memory()
{
    int32 size = 0;
    int32 tableIndex = 0;

    /*
        size : 32byte의 메모리풀을 1개 만들고 _poolTable에 0~32 tableIndex에 넣음
        size : 64byte의 메모리풀을 1개 만들고 _poolTable에 33~64 tableIndex에 넣음
        ...
        size : 1024byte의 메모리풀을 1개 만들고 _poolTable에 993~1024 tableIndex에 넣음
    */
    for (size = 32; size <= 1024; size += 32)
    {
        MemoryPool* pool = new MemoryPool(size);
        _pools.push_back(pool);
        while (tableIndex <= size)
        {
            _poolTable[tableIndex] = pool;
            tableIndex++;
        }
    }

    /*
        size : 1152byte의 메모리풀을 1개 만들고 _poolTable에 1025~1152 tableIndex에 넣음
        ...
    */
    for (; size <= 2048; size += 128)
    {
        MemoryPool* pool = new MemoryPool(size);
        _pools.push_back(pool);
        while (tableIndex <= size)
        {
            _poolTable[tableIndex] = pool;
            tableIndex++;
        }
    }

    for (; size <= 4096; size += 256)
    {
        MemoryPool* pool = new MemoryPool(size);
        _pools.push_back(pool);
        while (tableIndex <= size)
        {
            _poolTable[tableIndex] = pool;
            tableIndex++;
        }
    }
}

Memory::~Memory()
{
    for (MemoryPool* pool : _pools)
        delete pool;
    _pools.clear();
}

void* Memory::Allocate(int32 size)
{
    MemoryHeader* header = nullptr;
    const int32 allocSize = size + sizeof(MemoryHeader);

    if (allocSize > MAX_ALLOC_SIZE)
    {
        // 메모리 풀링 최대 크기를 벗어나면 일반 할당
        header = reinterpret_cast<MemoryHeader*>(::malloc(allocSize));
    }
    else
    {
        // 메모리 풀에서 꺼내온다
        header = _poolTable[allocSize]->Pop();
    }

    return MemoryHeader::AttachHeader(header, allocSize);
}

void Memory::Release(void* ptr)
{
    MemoryHeader* header = MemoryHeader::DetachHeader(ptr);
    const int32 allocSize = header->allocSize;
    ASSERT_CRASH(allocSize > 0);
    
    if (allocSize > MAX_ALLOC_SIZE)
    {
        // 메모리 풀링 최대 크기를 벗어나면 일반 해제
        ::free(header);
    }
    else
    {
        // 메모리 풀에 반납한다
        _poolTable[allocSize]->Push(header);
    }
}
```