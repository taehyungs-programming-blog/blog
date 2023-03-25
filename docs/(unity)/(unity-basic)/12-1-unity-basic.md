---
layout: default
title: "12. Coroutine"
parent: "(Basic)"
grand_parent: "(Unity ✨)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Coroutine의 필요성?

```csharp
void VeryComplicated()
{
    // 여기서 엄청나게복잡한 작업을 한다 가정하자
    for(int i = 0; i < 100000000; i++)
    {
        Debug.Log("Hello");
    }
}

void Update()
{
    // 과연 이 복잡한 작업을 Update에서 그냥 Call하는게 맞나?
    VeryComplicated();
}
```

* 추가로 매 Tick마다 필요한 작업인지도 중요하다
* 만약 매 Tick마다 필요한 작업이 아니라 **부분적으로 나눠서** 해도되는 작업이라면
* **Coroutine**을 써보자

---

## Coroutine 써보기

* **Coroutine** - 함수의 상태를 저장/복원이 가능하다
    * 오래걸리는 작업을 잠시 끊거나
    * 원하는 타이밍에 함수를 잠시 Stop

```csharp
class CoroutineTest : IEnumerable
{
    public IEnumerator GetEnumerator()
    {
        // IEnumerator의 return에는 yield이 필요하다
        yield return 1;
        yield return 2;
        yield return 3;
        yield return 4;
        yield return 5;

        // 종료관련
        yield return null;  // 만약에 아무것도 안넘기고 싶다면 null로 리턴하자
        yield break;        // 진짜로 종료하고자 한다면 break하자.
    }
}

CoroutineTest test = new CoroutineTest();
foreach(var t in test)  // (Tip) var에느 System.Object가 잡힌다
{
    Debug.Log(t);
    // 1, 2, 3, 4, 5 출력 됨.
    // GetEnumerator()를 호출할때마다. 순차적으로 호출됨을 알 수 있다.
}
```

```csharp
class Test
{
    public int Id = 0;
}

class CoroutineTest : IEnumerable
{
    public IEnumerator GetEnumerator()
    {
        yield return new Test() {Id = 1};
        yield return new Test() {Id = 2};
        yield return new Test() {Id = 3};
        yield return new Test() {Id = 4};
        yield return new Test() {Id = 5};
    }
}

CoroutineTest test = new CoroutineTest();
foreach(var t in test)
{
    // (Tip) object로 받을수 있음
    int value = ((Test)t).Id;
    Debug.Log(value);
}
```

---

## Coroutine Example - 1

* 사실 Client보다 Server에서 사용하기 좋음

```csharp
// Item을 만든다 가정해보자
void GenerateItem()
{
    // Item 생성

    // DB저장 (이게 오래걸림) 
        // 하지만 DB 응답정보를 받지않고 아래 로직을 처리할 경우
        // DB와 Client간에 데이터 오류가 발생한다.
        // 여기를 Coroutine으로 처리하면 된다.
            // Coroutine을 넣을경우 아래로직으로 넘어가기 전 다른 프로세스에게 처리를 넘길수 있음.

    // 로직

}
```

---

## Coroutine Example - 2

* 4초 후 폭파하는 스킬이 있다고 가정하자.

```csharp
float deltaTime = 0;
void ExplodeAfter4Seconds()
{
    // 매번 이렇게 더해서 Update에서 관리한다고?
        // 만약 스킬이 추가된다면? Update에 하나가 더 추가돼야하나?
    deltaTime += Time.deltaTime;
    if(deltaTime >= 4)
    {
        // ...
    }
}

void Update()
{
    ExplodeAfter4Seconds();
}
```

* 틱마다 체크하면 될꺼 같은데... 
* 만약 이 스킬을 쓰는 객체가 1000개라면?? 10000개라면 ??? 엄청난 부하이다.

```csharp
// ** 그래서 Unity가 준비했다. **

Coroutine co = StartCoroutine("ExplodeAfterSceconds", 4.0f);
// StopCoroutine(co);   // (Tip) 취소하고자 한다면?
if(co != null)
{
    StopCoroutine(co);
    co = null;
}

IEnumerator ExplodeAfterSceconds(float seconds)
{
    Debug.Log("ExplodeAfterSceconds - Enter");
    yield return new WaitForSeconds(seconds);
    Debug.Log("ExplodeAfterSceconds - Start");
}
```