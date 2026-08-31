---
layout: default
title: "virtual thread와 coroutine"
parent: "Kotlin"
nav_order: 1
---

JVM 진영에서 가벼운 동시성을 다룰 때 자주 비교되는 도구가 있다.
Java 21부터 정식 제공되는 **virtual thread**와 Kotlin의 **coroutine**이다.

---

## 한 줄 요약

둘 다 "스레드 하나가 I/O를 기다리는 동안 놀고 있는 문제"를 줄이기 위한 기술이다.
다만 virtual thread는 **블로킹 코드를 싸게 실행**하는 데 강하고, coroutine은 **논블로킹 흐름을 구조화**하는 데 강하다.

---

## 동작 계층

| 구분 | Virtual thread | Coroutine |
| --- | --- | --- |
| 제공 주체 | JVM | Kotlin 언어와 라이브러리 |
| 실행 단위 | 경량 스레드 | suspend 함수의 continuation |
| 코드 스타일 | 기존 동기식 코드와 잘 맞음 | suspend 기반 비동기 코드와 잘 맞음 |
| 블로킹 호출 | JVM이 가능한 경우 carrier thread에서 unmount | dispatcher thread를 그대로 점유 |
| 대표 장점 | 기존 blocking API를 크게 바꾸지 않고 확장성 확보 | structured concurrency, cancellation, timeout 표현이 자연스러움 |

Virtual thread는 JVM이 관리하는 경량 스레드다.
`Thread` API를 그대로 사용할 수 있고, JDBC나 `Thread.sleep()` 같은 블로킹 호출을 만나면 JVM이 가능한 경우 carrier thread에서 virtual thread를 내려놓는다.

Coroutine은 Kotlin 컴파일러가 `suspend` 함수를 상태 머신으로 변환해서 동작한다.
중단 지점에서 실행을 멈췄다가 나중에 이어서 실행할 수 있으며, 실제 실행 스레드는 dispatcher가 관리한다.

`suspend` 함수는 코루틴 런타임에게 "이 함수는 중간에 멈췄다가 나중에 이어서 실행될 수 있다"는 것을 알려준다.
실제 suspend 지점에서는 현재 스레드를 붙잡고 기다리지 않으므로, 그 스레드는 다른 작업을 할 수 있다.
다만 `suspend`가 붙었다고 자동으로 새 코루틴이 만들어지거나, blocking 코드가 자동으로 논블로킹으로 바뀌는 것은 아니다.

```kotlin
suspend fun loadUser(id: Long): User {
  return userClient.getUser(id)
}
```

---

## 블로킹 처리 방식

### Virtual thread

Virtual thread의 핵심은 기존 동기식 코드를 크게 바꾸지 않고도 블로킹 비용을 낮추는 것이다.
요청마다 스레드를 하나씩 쓰는 서버 코드나 JDBC 기반 코드처럼 이미 blocking API 중심으로 작성된 코드와 잘 맞는다.

```java
try (var executor = Executors.newVirtualThreadPerTaskExecutor()) {
  executor.submit(() -> {
    Thread.sleep(1000);
    return "done";
  });
}
```

이 코드는 여전히 동기식 코드처럼 보인다.
하지만 virtual thread가 대기 상태가 되면 carrier thread를 다른 작업에 넘길 수 있다.

### Coroutine

Coroutine은 `suspend` API를 사용할 때 강점이 드러난다.
I/O 대기 중에는 현재 실행을 중단하고, 완료 후 continuation을 다시 실행한다.

```kotlin
suspend fun loadUsers(ids: List<Long>): List<User> = coroutineScope {
  ids.map { id ->
    async { userClient.getUser(id) }
  }.awaitAll()
}
```

반대로 `suspend` 함수 안에서 JDBC 같은 일반 블로킹 호출을 그대로 호출하면 dispatcher thread가 묶인다.
이 경우 `withContext(Dispatchers.IO)`로 블로킹 호출 구간만 I/O 작업용 스레드에서 실행하도록 분리한다.

---

## 예시로 비교하기

### 1. 기존 blocking API를 많이 쓰는 서버

JDBC repository나 동기 HTTP client를 이미 사용하고 있다면 virtual thread가 자연스럽다.
코드는 여전히 순차적인 blocking 코드처럼 읽히지만, 대기 중인 virtual thread는 carrier thread를 계속 붙잡고 있지 않는다.

```java
List<User> findUsers(List<Long> ids) throws Exception {
  try (var executor = Executors.newVirtualThreadPerTaskExecutor()) {
    List<Future<User>> futures = ids.stream()
        .map(id -> executor.submit(() -> userRepository.findById(id)))
        .toList();

    List<User> users = new ArrayList<>();
    for (var future : futures) {
      users.add(future.get());
    }
    return users;
  }
}
```

여기서 `userRepository.findById(id)`가 JDBC 기반 blocking 호출이어도 코드 구조를 크게 바꿀 필요가 없다.
기존 request-per-thread 스타일을 유지하면서 동시 요청 처리 비용을 낮추고 싶을 때 잘 맞는다.

### 2. suspend API를 조합하는 Kotlin 코드

이미 사용하는 client가 `suspend` 함수를 제공한다면 coroutine이 더 자연스럽다.
여러 비동기 작업을 `async`로 시작하고, 필요한 시점에 `await`로 결과를 합칠 수 있다.

```kotlin
suspend fun loadProfile(userId: Long): Profile = coroutineScope {
  val user = async { userClient.getUser(userId) }
  val orders = async { orderClient.getOrders(userId) }
  val coupons = async { couponClient.getCoupons(userId) }

  Profile(
    user = user.await(),
    orders = orders.await(),
    coupons = coupons.await(),
  )
}
```

이 방식의 장점은 단순히 동시에 실행된다는 점만이 아니다.
`coroutineScope` 안의 작업 중 하나가 실패하면 나머지 child coroutine도 함께 취소되는 식으로 생명주기가 묶인다.

### 3. coroutine 안에서 blocking API를 호출하는 경우

다음 코드는 `suspend` 함수 안에 있지만 안전한 논블로킹 코드가 아니다.
`jdbcRepository.findById(id)`가 blocking 호출이면, 실행 중인 dispatcher thread를 그대로 점유한다.

```kotlin
suspend fun findUser(id: Long): User {
  return jdbcRepository.findById(id)
}
```

blocking API를 coroutine 안에서 써야 한다면 최소한 blocking 작업용 dispatcher로 경계를 분리한다.
여기서 dispatcher는 coroutine을 어떤 스레드에서 실행할지 정하는 실행 장소다.

- `Dispatchers.Default`: CPU 계산 작업용
- `Dispatchers.IO`: 파일, DB, 네트워크 같은 blocking I/O 작업용
- `Dispatchers.Main`: Android UI 작업용

`withContext(Dispatchers.IO)`는 "이 블록 안의 코드는 I/O 작업용 스레드에서 실행해줘"라는 뜻이다.
DB 조회처럼 오래 기다릴 수 있는 blocking 작업만 잠깐 `Dispatchers.IO`로 옮겨 실행하고, 끝나면 결과를 가지고 원래 coroutine 흐름으로 돌아온다.

```kotlin
suspend fun findUser(id: Long): User = withContext(Dispatchers.IO) {
  jdbcRepository.findById(id)
}
```

이 코드는 JDBC를 논블로킹으로 바꾸지는 않는다.
다만 기본 dispatcher를 blocking 작업으로 막지 않도록 격리하는 효과가 있다.

### 4. timeout과 cancellation이 중요한 경우

작업 전체에 제한 시간을 걸거나, 상위 작업이 취소될 때 하위 작업도 같이 멈춰야 한다면 coroutine의 표현력이 좋다.

```kotlin
suspend fun loadWithTimeout(userId: Long): Profile =
  withTimeout(1_000) {
    loadProfile(userId)
  }
```

Virtual thread에서도 `Future.cancel()`이나 interrupt를 사용할 수 있지만, Kotlin 코드 안에서 작업 관계를 구조적으로 표현하는 데는 coroutine이 더 직접적이다.

---

## 선택 기준

### Virtual thread가 어울리는 경우

- 기존 Java/Kotlin 서버 코드가 blocking API 중심이다.
- JDBC, 파일 I/O, 외부 HTTP 호출처럼 동기식 라이브러리를 많이 사용한다.
- 코드 구조를 크게 바꾸지 않고 request-per-thread 모델을 유지하고 싶다.
- 높은 동시 접속에서 OS thread 비용을 줄이는 것이 목적이다.

### Coroutine이 어울리는 경우

- Kotlin 코드베이스이고 `suspend` API를 적극적으로 사용할 수 있다.
- cancellation, timeout, parent-child job 같은 구조화된 동시성이 중요하다.
- Ktor, R2DBC, WebClient 등 비동기/논블로킹 라이브러리와 함께 쓴다.
- 여러 비동기 작업을 조합하고 흐름을 명확히 표현해야 한다.

---

## 같이 쓸 수 있나?

같이 쓸 수는 있다.
다만 둘의 역할을 섞어 생각하면 오히려 복잡해진다.

- 기존 blocking 코드를 유지하면서 동시성 비용을 낮추고 싶다면 virtual thread를 우선 고려한다.
- Kotlin 내부에서 비동기 작업의 생명주기와 취소 전파를 명확히 다루고 싶다면 coroutine을 우선 고려한다.
- coroutine 안에서 blocking API를 호출해야 한다면 dispatcher 선택을 명확히 한다.
- virtual thread 위에서도 coroutine을 실행할 수 있지만, 이 경우 왜 두 모델이 모두 필요한지 먼저 따져야 한다.

---

## 주의할 점

- Virtual thread는 CPU 작업을 빠르게 만드는 기술이 아니다. I/O 대기 시간이 긴 작업에서 효과가 크다.
- 모든 블로킹이 항상 carrier thread에서 깔끔하게 분리되는 것은 아니다. 특정 native 호출이나 synchronized 구간에서는 carrier thread가 묶일 수 있다.
- Coroutine은 블로킹 호출을 자동으로 논블로킹으로 바꿔주지 않는다.
- Coroutine의 장점은 `suspend` API, dispatcher, structured concurrency를 일관되게 사용할 때 커진다.

정리하면, virtual thread는 **동기식 코드를 유지하면서 블로킹 비용을 낮추는 JVM 기능**이고,
coroutine은 **Kotlin에서 비동기 흐름을 명확하게 표현하기 위한 언어/라이브러리 모델**이다.
