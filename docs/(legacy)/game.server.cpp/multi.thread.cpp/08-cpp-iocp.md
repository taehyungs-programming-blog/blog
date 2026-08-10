---
layout: default
title: "08. 컴퓨터 구조(Cash, CPU pipeline)"
parent: "(C++) Multi Thread"
grand_parent: "Game Server 👾"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Cash

```
<CPU> - <RAM>
```

* CPU와 RAM의 물리적 거리로 인해서 CPU에서 연산할 **데이터를 매번 RAM에서 가져다쓰는 것은 한계**가 있다.
* 따라서 캐시를 두어 CPU에서 처리할 데이터를 **미리 끌어 놓는다**

```
** 대략 아래와 같은 구조 **

    <<CPU 내부>>
    ALU(CPU Core)
      레지스터
       L1 캐시
       L2 캐시
         |
         |
      <<RAM>>
```

```cpp
int32 buffer[10000][10000];

int main()
{
    // 초기화
    memset(buffer, 0, sizeof(buffer));

    int64 start = GetTickCount64();

    int64 sum = 0;
    for(int32 i = 0; i < 10'000; i++)
        for(int32 j = 0 ; j < 10'000; j++)
            sum += buffer[i][j];

    uin64 end = GetTickCount64();
    cout << "Elapsed Tick" << (end - start) << endl;
}
```

* 동일하게 구성한 후 j, i 순서만 변경

```cpp
int64 start = GetTickCount64();

int64 sum = 0;
for(int32 i = 0; i < 10'000; i++)
    for(int32 j = 0 ; j < 10'000; j++)
        sum += buffer[j][i];        // 여기

uin64 end = GetTickCount64();
cout << "Elapsed Tick" << (end - start) << endl;
```

* 테스트해보면 속도차이가 3~4배 정도 난다.(첫번째가 빠름)
* 신기하네? 👉 **연속된 메모리가 캐쉬로 올라가기때문**

---

## pipe line

```cpp
int32 x = 0;
int32 y = 0;
int32 r1 = 0;
int32 r2 = 0;

volatile bool ready;

void Thread_1()
{
    while(!ready)
        ;

    y = 1; // Store y
    r1 = x; // Load x
}

void Thread_2()
{
    while(!ready)
        ;

    x = 1; // Store x
    r2 = y; // Load y
}

int main()
{
    int32 count = 0;

    while(true)
    {
        ready = false;
        count++;

        x = y = r1 = r2 = 0;

        thread t1(Thread_1);
        thread t2(Thread_2);
        ready = true;       // t1, t2가 동시에 동작하게 하기위해서 넣음.
        t1.join();
        t2.join();

        if(r1 == 0 && r2 == 0)
            break;
    }

    cout << count << "번만에 빠져나옴" << endl;

}
```

* 해보면 1000~2000번 안에 빠져나오게 된다.
* ??? 0을 넣지도 않는데 어떻게 빠져나올수가 있을까??
* 일단 문제는 두 가지다

* **가시성**
    * CPU는 각 Core마다 각자의 캐쉬가 있다.
    * 따라서 Store, Load된 데이터가 실제 RAM의 값인지 캐쉬의 값인지 알 방법이 없다
* **코드 재배치**
    * 컴파일러 or CPU가 코드를 자신의 환경에 맞게 재배치(최적화)하는 경우가 발생한다.
    * 이런 코드 재배치를 하는 이유가 pipe-line과 연관된다. 
    
```cpp 
void Thread_1() { 
    while(!ready) 
    ;

    y = 1; // Store y 
    r1 = x; // Load x 
    }
```

```cpp
// 이런코드를 컴파일러가

void Thread_1() { 
    while(!ready) 
    ;
    // 이렇게 배치할수 있다.
    r1 = x; // Load x
    y = 1; // Store y   }   
```

* CPU에선 연산을 다음 단계를 거쳐서 진행한다.
    * Fetch(코드를 가져오고) - Decode(해석후) - Execute(시행) - Write-back(결과를 통보)
* 이런 파이프라인을 효율적으로 돌리기위해서 컴파일러 혹은 CPU가 파이프라인에 효율적이게 코드재배치를 진행하게 된다.
* `volitile`로 재배치를 하지말아달라 막아둘 수 있음.
* `std::atomic`을 사용해도 됨.
    * Windows의 경우 InterLockInterOOO을 쓰기도 하는데 표준이 나온이상 atomic을 쓰는것을 추천

