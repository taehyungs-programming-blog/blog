---
layout: default
title: "[구현] 메모리 Allocator - 1 (new, delete 커스터 마이징)"
parent: "(C++ IOCP)"
grand_parent: "Game Server 👾"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get This Code 🌎](https://github.com/EasyCoding-7/Windows_Game_Server_Tutorial/tree/RA-Tag-06)

---

## 근본적 질문

😺 **Memory Allocator** - 메모리를 단순 new, delete로 관리하지말고 미리 Allocate해둔 후 관리해 보자.<br>

* 메모리 Allocation이 왜 필요할까? 👉
    * 우선 Windows기반에서는 메모리관리를 OS에서 해준다.
    * 할당해야할 메모리가 많아진다면 OS의 커널영역에서 새로운 메모리 할당을 하며 컨텍스트 스위칭비용이 발생하는데 그 비용을 감소시키기 위함이 있고
    * 또한 메모리 파편화 현상을 방지하는데 내가 메모리를 Allocate해서 관리하면 좋기 때문이다.
* 우선 이번강에서는 위 문제를 해결한순 없다. 단, new, delete를 이렇게 개발자가 커스터마이징 가능하다는 것을 이해하자

---

## 구현 1차

😺 그럼 구현을 해보자.

```cpp
// new, delete를 어떻게 커스터마이징 할까

// new operator overloading (Global)
void* operator new(size_t size)
{
	cout << "new! " << size << endl;
	void* ptr = ::malloc(size);
	return ptr;
}

void operator delete(void* ptr)
{
	cout << "delete!" << endl;
	::free(ptr);
}

void* operator new[](size_t size)
{
	cout << "new[]! " << size << endl;
	void* ptr = ::malloc(size);
	return ptr;
}

void operator delete[](void* ptr)
{
	cout << "delete![]" << endl;
	::free(ptr);
}

// 생각보다 간단...
// 단, 이렇게 전역으로 구현하면 의도치 않은 new와 delete에도 영향을 줄 수 있다.
```

---

## 구현 2차

😺 그럼 class내에서 만들어야 할까?

```cpp
class Knight
{
public:
	Knight()
	{
		cout << "Knight()" << endl;
	}

	Knight(int32 hp) : _hp(hp)
	{
		cout << "Knight(hp)" << endl;
	}

	~Knight()
	{
		cout << "~Knight()" << endl;
	}

    // 이렇게 객체안에 넣어서도 구현가능
	static void* operator new(size_t size)
	{
		cout << "Knight new! " << size << endl;
		void* ptr = ::malloc(size);
		return ptr;
	}

	static void operator delete(void* ptr)
	{
		cout << "Knight delete!" << endl;
		::free(ptr);
	}

	int32 _hp = 100;
	int32 _mp = 10;
};
```

🙀 모든 클래스에 넣는건 좀...

---

## 구현 3차

```cpp
// 새로운 new, delete를 만들어 보자

#ifdef _DEBUG
#define xalloc(size)		BaseAllocator::Alloc(size)
#define xrelease(ptr)		BaseAllocator::Release(ptr)
#else
#define xalloc(size)		BaseAllocator::Alloc(size)
#define xrelease(ptr)		BaseAllocator::Release(ptr)
#endif
```

```cpp
void* BaseAllocator::Alloc(int32 size)
{
	return ::malloc(size);
}

void BaseAllocator::Release(void* ptr)
{
	::free(ptr);
}
```

```cpp
#pragma once
#include "Allocator.h"

template<typename Type, typename... Args>
Type* xnew(Args&&... args)
{
	Type* memory = static_cast<Type*>(xalloc(sizeof(Type)));
	new(memory)Type(forward<Args>(args)...); // placement new - 여기가 헷갈린다(지연 생성)
    // new(memory) : 메모리는 이미할당되어 있으니 할당하지말고
    // Type(forward<Args>(args)...) : 요렇게 생성자만 호출해 줘
	return memory;
}

template<typename Type>
void xdelete(Type* obj)
{
	obj->~Type();
	xrelease(obj);
}
```

* 참고로 placement new를 굳이 하는 이유는 malloc/free은 생성자/소멸자의 호출이 없다([참고사이트 🌎](https://rieulpost.tistory.com/52))

😺 실사용은 이렇게

```cpp
int main()
{	
	Knight* knight = xnew<Knight>(100);

	xdelete(knight);
}
```