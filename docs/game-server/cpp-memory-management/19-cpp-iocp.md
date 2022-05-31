---
layout: default
title: "[이론] 메모리 오염에 관하여(VirtualAlloc, VirtualFree)"
parent: "(C++) Memory Management"
grand_parent: "Game Server 👾"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 메모리 오염 Example

### Example 1

💩 삭제 후 재접근 1

```cpp
Knight k1 = new Knight();

k1->hp = 200;
k1->mp = 150;

delete k1

// ...

k1->hp = 100;       // ??? -> 여기서 crash가 날 수도 안 날 수도 있다.

// 만약 k1->hp 메모리 공간에 다른 메모리가 써진다면??? -> 그 데이터를 오염시킬 수 있다.
```

### Example 2

💩 삭제 후 재접근 2

```cpp
vector<int32> v{1,2,3,4,5};

for(int32 i = 0; i < 5; i++)
{
    int32 value = v[i];

    // ...

    if(/*something*/)
    {
        v.clear();
        // break;   // 깜빡하고 break를 안함
        // clear했는데 break를 안해서 for문에 다시 돌아가 잘못된 메모리에 접근
    }
}
```

### Example 3

💩 캐스팅 미스

```cpp
class Player
{

};

class Knight : public Player
{

};

Player* p = new Player();
Knight* k = static_cast<Knight*>(p);    
// dynamic_cast로 잡아야하지만 성능이슈로 static_cast로 잡았는데 잘못 캐스팅함.
```

---

## 사용자 영역 메모리에 대해서 

😺 다양한 문제가 있구나? 어떻게 해결해야 할까?<br>
😺 우선 OS의 메모리에 대해 이해해야한다.

```cpp
// 가상 메모리 기본!

int* num = new int;
int64 add = reinterpret_cast<int64>(num);   

// 여기서 나온주소대로 다른 프로세스에서 접근이 가능할까?
// 당연하지만 안된다. 메모리 주소(가상주소)는 각 프로세스마다 관리하게 된다.

delete num;
```

```cpp
// 어떤 프로세스에서 2GB메모리를 쓴다고 가정해보자.
// 2GB [                                     ]
// 그럼 가상메모리에 접근하기 위해서 메모리주소가 몇개가 필요할까?
// 1바이트에 접근하기 위해 1바이트가 필요하다면 해당프로세스에 2GB메모리할당을 위해 접근을 위한 메모리를 2GB할당해야하는 바보 같은 일이 발생한다.
// 따라서 메모리 공간보다 크게 접근 메모리를 할당하는데 이를 페이징이라한다.
// -> 보통 4kb이다.
// 페이징 마다 read/write 권한을 줄 수 있다.
// 실제로 확인해보자.

SYSTEM_INFO info;
::GetSystemInfo(&info);
info.dwPageSize;                // 4KB
info.dwAllocationGranularity;   // 64KB
```

```cpp
class StompAllocator
{
    enum { PAGE_SIZE = 0x1000 };        // 4096
    //...

void* StompAllocator::Alloc(int32 size)
{
    const int64 pageCount = (size + PAGE_SIZE - 1) / PAGE_SIZE; // 몇개의 페이지를 할당할까?
    const int64 dataOffset = pageCount * PAGE_SIZE - size;      // 메모리 오버플로 방지용
    void* baseAddress = ::VirtualAlloc(NULL,            // 알아서 메모리 공간을 잡아달라
                        pageCount * PAGE_SIZE,          // 메모리 크기
                        MEM_RESERVE | MEM_COMMIT,       // MEM_RESERVE(예약), MEM_COMMIT(사용)
                        PAGE_READWRITE);                // 권한

    return static_cast<void*>(static_cast<int8*>(baseAddress) + dataOffset);
    // 실제 메모리 사용은 할당된 메모리의 마지막 부분만 쓰게 한다.
    // [               [ 여기만 쓴다 ]]
}

void StompAllocator::Release(void* ptr)
{
    const int64 address = reinterpret_cast<int64>(ptr);
    const int64 baseAddress = address - (address % PAGE_SIZE);
    ::VirtualFree(reinterpret_cast<void*>(baseAddress), 0, MEM_RELEASE);
}

// new, delete와 차이점은?
// VirtualFree는 실제로 메모리 공간을 밀어버린다.
    // 메모리 오염문제를 방지할 수 있다.
```

* `VirtualAlloc`, `VirtualFree`를 사용시 메모리 오염의 문제는 해결됐지만
    * 메모리 파편화, 재사용의 문제는 해결되지 않았다.
    * 역시 뒤에서 다시 설명한다.

---

* 32bits 운영체제 기준으로 주소값은 2^32=4,294,967,296개의(4G) 주소값을 가리킬 수 있다
    * 윈도우가 어플리케이션에 할당해주는 메모리가 4G[커널(2G) + 유저(2G)]이다.
    * 만약 포인터 변수 하나(4byte(32비트기준))가 유저영역 메모리를 4byte단위로 가리킨다면 유저메모리를 모두 가리키기 위해선 2G포인터 변수가 필요해진다(비효율적)
    * 따라서 덩어리(페이지)단위로 끊어서 메모리를 할당하고 관리하게 사용
* 참고로 64bit운영체제의 포인터의 크기는 8바이트(64bit)이다.(당연)
* 32bit에서 int형은 4바이트이다. 그럼? 64bit에서 int형은 8바이트일까?
    * 그럴리가 없음. 그럼 운영체제의 bit가 바뀔때마다 int형의 표현단위가 달라지게 되는데 어떤 오류를 유발할지 모른다.
    * int형은 그냥 4바이트(CPU벤더사에 따라 달라질 수 있으나 달라질 확률은 극히 낮음)