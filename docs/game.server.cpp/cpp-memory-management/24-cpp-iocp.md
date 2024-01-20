---
layout: default
title: "[구현] ObjectPool"
parent: "(C++) Memory Management"
grand_parent: "Game Server 👾"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get This Code 🌎](https://github.com/EasyCoding-7/Windows_Game_Server_Tutorial/tree/RA-Tag-12)

---

* 기존 메모리풀의 아쉬운점 : 
	* 비슷한 크기의 메모리는 같은 메모리공간에 할당된다.
	* 메모리 오염의 원인이 될 수 있다.
		* (단일 클래스가 동일한 메모리 풀을 사용하게 된다면 해결될 것 -> 오브젝트 풀)


```cpp
#pragma once
#include "Types.h"
#include "MemoryPool.h"

template<typename Type>
class ObjectPool
{
public:
	template<typename... Args>
	static Type* Pop(Args&&... args)
	{
#ifdef _STOMP
		MemoryHeader* ptr = reinterpret_cast<MemoryHeader*>(StompAllocator::Alloc(s_allocSize));
		Type* memory = static_cast<Type*>(MemoryHeader::AttachHeader(ptr, s_allocSize));
#else
		Type* memory = static_cast<Type*>(MemoryHeader::AttachHeader(s_pool.Pop(), s_allocSize));
#endif		
		new(memory)Type(forward<Args>(args)...); // placement new
		return memory;
	}

	static void Push(Type* obj)
	{
		obj->~Type();
#ifdef _STOMP
		StompAllocator::Release(MemoryHeader::DetachHeader(obj));
#else
		s_pool.Push(MemoryHeader::DetachHeader(obj));
#endif
	}

    // Pop을 할경우 항상 Push를 통해 메모리 해제를 해야하니
    // 차라리 Shared_Ptr로 관리해보자.
    // shared_ptr<Player> player = MakeShared<Knight>(); 이런식으로 쓴다
	static shared_ptr<Type> MakeShared()
	{
		shared_ptr<Type> ptr = { Pop()/*포인터 주소*/, Push/*소멸자를 지정*/ };
		return ptr;
	}

private:
    // static으로 선언함으로써 클래스(오브젝트)당 하나씩 갖게된다.
	static int32		s_allocSize;
	static MemoryPool	s_pool;
};

template<typename Type>
int32 ObjectPool<Type>::s_allocSize = sizeof(Type) + sizeof(MemoryHeader);

template<typename Type>
MemoryPool ObjectPool<Type>::s_pool{ s_allocSize };
```
