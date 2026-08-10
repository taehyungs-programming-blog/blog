---
layout: default
title: "[구현] Memory pool - 2"
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

* [Get This Code 🌎](https://github.com/EasyCoding-7/Windows_Game_Server_Tutorial/tree/RA-Tag-10)

---

## 기존 코드의 문제점?

* Memory Pool에서 Lock을 건다

```cpp
void MemoryPool::Push(MemoryHeader* ptr)
{
    WRITE_LOCK;
    
    // ...

MemoryHeader* MemoryPool::Pop()
{
    MemoryHeader* header = nullptr;
    {
        WRITE_LOCK;

        // ...
```

* queue에 메모리 헤더를 넣는데, queue자체가 동적할당을 쓰기에 이 부분도 Memory Pool에 넣고 싶다

```cpp
class MemoryPool
{
    // ...

    USE_LOCK;
    // 그냥 queue말고 Memory Pool을 쓰는 queue에 넣고자 한다.
    queue<MemoryHeader*> _queue;
};
```

---

## Memory Pool을 쓰는 queue 구현

```cpp
#pragma once

// -------------------
//		1차 시도
// -------------------

struct SListEntry
{
	SListEntry* next;
};

struct SListHeader
{
	SListEntry* next = nullptr;
};

void InitializeHead(SListHeader* header);
void PushEntrySList(SListHeader* header, SListEntry* entry);
SListEntry* PopEntrySList(SListHeader* header);
```

```cpp
// []
// Header[ next ]
void InitializeHead(SListHeader* header)
{
	header->next = nullptr;
}

void PushEntrySList(SListHeader* header, SListEntry* entry)
{
    // header->next : 가장 최근 데이터를 의미
    // entry->next : 직전 데이터를 의미

	entry->next = header->next; // entry->next에는 이전 최근데이터였던 header->next를 넣으며 직전데이터로 만든다
	header->next = entry;       // header->next최근데이터로 만들기
}

SListEntry* PopEntrySList(SListHeader* header)
{
	SListEntry* first = header->next;   // header->next최근데이터를 받고

	if (first != nullptr)
		header->next = first->next;     // first->next(entry->next) 직전데이터를 header->next에 넣음

	return first;
}
```

```cpp
// 실 사용은 이렇게
class Data // : public SListEntry
{
public:
	SListEntry _entry;
	
    int32 _hp;
    int32 _mp;
};

int main()
{
	SListHeader header;
	InitializeHead(&header);

    Data* data = new Data();
    PushEntrySList(&header, (SListEntry*)data);

    Data* popData = (Data*)PopEntrySList(&header);
}
```

💩 문제점 - **좋긴한데 멀티쓰레드 환경에 적합하진 않다.**

---

```cpp
// -------------------
//		2차 시도
// -------------------

struct SListEntry
{
	SListEntry* next;
};

struct SListHeader
{
	SListEntry* next = nullptr;
};

void InitializeHead(SListHeader* header);
void PushEntrySList(SListHeader* header, SListEntry* entry);
SListEntry* PopEntrySList(SListHeader* header);
```

```cpp
void InitializeHead(SListHeader* header)
{
	header->next = nullptr;
}

void PushEntrySList(SListHeader* header, SListEntry* entry)
{
	entry->next = header->next;

    // header->next = entry 하는 과정에서 다른 Thread가 영향을 줄까 두렵다.
	while (::InterlockedCompareExchange64((int64*)&header->next, 
                                        (int64)entry, 
                                        (int64)entry->next) == 0)
    /*
    if(header->next == entry->next) {
        header->next = entry;
    }
    else {
        entry->next = header->next;
    }
    */
	{

	}
}

// [][]
// Header[ next ]
SListEntry* PopEntrySList(SListHeader* header)
{
	SListEntry* expected = header->next;

	// ABA Problem
    // 이런 상황이 극적으로 발생할 수 있다.
    // header->next == expected 를 체크하는데 expected가 우연히 아주 우연히 삭제되었다 동일한 주소에 할당된 경우,
    // 단순 주소체크만으론 완벽하게 멀티쓰레드 프리하다고 말 할 수 없다.
        // 사실 가능성이 있는게 Memory-pool을 만들어 쓰기때문에 가능성이 없지는 않음.
	while (expected && ::InterlockedCompareExchange64((int64*)&header->next, 
                                                    (int64)expected->next, 
                                                    (int64)expected) == 0)
    /*
    if(header->next == expected) {
        header->next = expected->next;
    }
    else {
        expected = header->next;
    }
    */
	{

	}

	return expected;
}
```

---

단일 포인터만을 Compare And Swap을 하지말라!

```cpp
// -------------------
//		3차 시도
// -------------------

#ifndef DECLSPEC_ALIGN
#if (_MSC_VER >= 1300) && !defined(MIDL_PASS)
#define DECLSPEC_ALIGN(x)   __declspec(align(x))        // x 바이트 정렬
#else
#define DECLSPEC_ALIGN(x)
#endif
#endif

// ...

DECLSPEC_ALIGN(16)      //16 바이트 정렬
struct SListEntry
{
	SListEntry* next;
};

DECLSPEC_ALIGN(16)
struct SListHeader
{
	SListHeader()
	{
		alignment = 0;
		region = 0;
	}

	union
	{
		struct
		{
			uint64 alignment;       // 64 bits
			uint64 region;          // 64 bits
		} DUMMYSTRUCTNAME;
		struct
		{
			uint64 depth : 16;      // 16 bits
			uint64 sequence : 48;   // 48 bits
			uint64 reserved : 4;    // 4 bits
			uint64 next : 60;       // 60 bits
		} HeaderX64;
	};
};

void InitializeHead(SListHeader* header);
void PushEntrySList(SListHeader* header, SListEntry* entry);
SListEntry* PopEntrySList(SListHeader* header);
```

```cpp
void InitializeHead(SListHeader* header)
{
	header->alignment = 0;
	header->region = 0;
}

void PushEntrySList(SListHeader* header, SListEntry* entry)
{
	SListHeader expected = {};
	SListHeader desired = {};
	
	// SListEntry는 16 바이트 정렬이 되어있다.
    // 16 바이트 정렬이 되어있단 말은 최하위 4비트는 0000이 되어있을것이다.
    // why? 0001 0000 = 16 / 0010 0000 = 32 ...

    // desired.HeaderX64.next는 60bits이기에 사용안되는 최하위 4비트를 민다
	desired.HeaderX64.next = (((uint64)entry) >> 4);

	while (true)
	{
		expected = *header;

		// 이 사이에 변경될 수 있다

		entry->next = (SListEntry*)(((uint64)expected.HeaderX64.next) << 4);
		desired.HeaderX64.depth = expected.HeaderX64.depth + 1;
		desired.HeaderX64.sequence = expected.HeaderX64.sequence + 1;

		if (::InterlockedCompareExchange128((int64*)header, 
                                        desired.region, 
                                        desired.alignment, 
                                        (int64*)&expected) == 1)
        /*
        if(header == expected)
        {
            header = desired;
        }
        else
        {
            header = expected;
        }
        */
			break;
	}
}

SListEntry* PopEntrySList(SListHeader* header)
{
	SListHeader expected = {};
	SListHeader desired = {};
	SListEntry* entry = nullptr;

	while (true)
	{
		expected = *header;

		entry = (SListEntry*)(((uint64)expected.HeaderX64.next) << 4);
		if (entry == nullptr)
			break;

		// Use-After-Free
		desired.HeaderX64.next = ((uint64)entry->next) >> 4;
		desired.HeaderX64.depth = expected.HeaderX64.depth - 1;
		desired.HeaderX64.sequence = expected.HeaderX64.sequence + 1;

		if (::InterlockedCompareExchange128((int64*)header, desired.region, desired.alignment, (int64*)&expected) == 1)
			break;
	}

	return entry;
}
```
