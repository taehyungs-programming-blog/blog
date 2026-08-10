---
layout: default
title: "[구현] 메모리 Allocator - 2 (Stomp, STL Allocator)"
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

* [Get This Code(Stomp) 🌎](https://github.com/EasyCoding-7/Windows_Game_Server_Tutorial/tree/RA-Tag-07)
* [Get This Code(STL) 🌎](https://github.com/EasyCoding-7/Windows_Game_Server_Tutorial/tree/RA-Tag-08)

---

## Stomp Allocator

🦄 `VirtualAlloc`, `VirtualFree`를 활용해 new, delete를 커스텀해보자.

```cpp
void* StompAllocator::Alloc(int32 size)
{
    const int64 pageCount = (size + PAGE_SIZE - 1) / PAGE_SIZE;
    const int64 dataOffset = pageCount * PAGE_SIZE - size;
    void* baseAddress = ::VirtualAlloc(NULL, pageCount * PAGE_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    return static_cast<void*>(static_cast<int8*>(baseAddress) + dataOffset);
}

void StompAllocator::Release(void* ptr)
{
    const int64 address = reinterpret_cast<int64>(ptr);
    const int64 baseAddress = address - (address % PAGE_SIZE);
    ::VirtualFree(reinterpret_cast<void*>(baseAddress), 0, MEM_RELEASE);
}
```

---

## STL Allocator

😺 STL container도 개발자가 직접 메모리 allocation할 수 있을까??<Br>
😺 STL 생성자를 따라가보면 Allocator를 받아줌을 알수 있다.

```cpp
// ...
// CLASS TEMPLATE vector
template <class _Ty, class _Alloc = allocator<_Ty>>
class vector { // varying size array of values
private:
// ...
```

😺 STL에서 사용가능한 Allocator를 만들어보자.

```cpp
template<typename T>
class StlAllocator
{
public:
    // 일단 value_type을 STLAllocator에 저장해둔다.
    using value_type = T;
    StlAllocator() { }
    // 복사생성자의 형식을 만들어둔다.
    template<typename Other>
    StlAllocator(const StlAllocator<Other>&) { }
    // 실질적으로 데이터를 할당 헤제하는 함수를 생성한다.(xalloc, xrelease는 이전강의 참조)
    T* allocate(size_t count)
    {
        const int32 size = static_cast<int32>(count * sizeof(T));
        return static_cast<T*>(xalloc(size));
    }
    void deallocate(T* ptr, size_t count)
    {
        xrelease(ptr);
    }
};
```

```cpp
// ...
#ifdef _DEBUG
#define xalloc(size)        StompAllocator::Alloc(size)
#define xrelease(ptr)       StompAllocator::Release(ptr)
#else
// ...
```

😺 사용의 편의성을 위해서 매크로를 만들어 사용하자

```cpp
// ...
template<typename Type>
using Vector = vector<Type, StlAllocator<Type>>;
template<typename Type>
using List = list<Type, StlAllocator<Type>>;
template<typename Key, typename Type, typename Pred = less<Key>>
using Map = map<Key, Type, Pred, StlAllocator<pair<const Key, Type>>>;
template<typename Key, typename Pred = less<Key>>
using Set = set<Key, Pred, StlAllocator<Key>>;
template<typename Type>
using Deque = deque<Type, StlAllocator<Type>>;
template<typename Type, typename Container = Deque<Type>>
using Queue = queue<Type, Container>;
template<typename Type, typename Container = Deque<Type>>
using Stack = stack<Type, Container>;
template<typename Type, typename Container = Vector<Type>, typename Pred = less<typename Container::value_type>>
using PriorityQueue = priority_queue<Type, Container, Pred>;
// String도 Allocator를 할당
using String = basic_string<char, char_traits<char>, StlAllocator<char>>;
using WString = basic_string<wchar_t, char_traits<wchar_t>, StlAllocator<wchar_t>>;
template<typename Key, typename Type, typename Hasher = hash<Key>, typename KeyEq = equal_to<Key>>
using HashMap = unordered_map<Key, Type, Hasher, KeyEq, StlAllocator<pair<const Key, Type>>>;
template<typename Key, typename Hasher = hash<Key>, typename KeyEq = equal_to<Key>>
using HashSet = unordered_set<Key, Hasher, KeyEq, StlAllocator<Key>>;
```