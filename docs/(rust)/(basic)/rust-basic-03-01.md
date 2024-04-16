---
layout: default
title: "03-01. rust와 GC / rust변수의 핵심"
parent: "(Rust 기초)"
grand_parent: "(Rust 🦀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## rust와 GC

* 선결론 rust에는 GC가 없다
* 그럼 동적할당(heap할당)하는 `Box, Vec, String` 와 같은 라이브러리는 언제 할당해제 되나?
    * 스코프를 벗어날때,

```rust
{
    let b = Box::new(5); // 힙에 5를 저장
    // b의 스코프가 끝나는 지점
} // 여기서 b는 스코프를 벗어나며, Box<T>의 drop 트레이트가 호출되어 힙 메모리를 해제합니다.

{
    let s = String::from("hello");
    // s의 스코프가 끝나는 지점
} // 여기서 s는 스코프를 벗어나며, String의 drop 트레이트가 호출되어 힙 메모리를 해제합니다.
```

* 동적으로 할당이 필요할 경우 `Box, Vec, String`를 쓰자.

---

## rust변수의 핵심

* 소유권(Ownership), 빌림(Borrowing), 라이프타임(Lifetimes)

### 소유권(Ownership)

* C++의 `std::move`라 보면된다.
* 자원경합(race condition)을 걱정할텐데 이건 이후강의에서 더 설명(아직 설명하긴 이르다)
* 유효성을 특정변수에서 갖기에 null exception과 같은 상황을 방지가능.

```rust
fn main() {
    let s1 = String::from("Hello");
    let s2 = s1; // s1의 소유권이 s2로 이동됩니다.

    // println!("{}", s1); // 컴파일 오류! s1은 더 이상 유효하지 않습니다.
    println!("{}", s2); // 정상 작동
}
```

### 빌림(Borrowing)

* 읽기만 가능하다. semaphore라 생각하면 될 듯?

```rust
fn main() {
    let mut s = String::from("hello");

    let r1 = &s; // 불변 참조 생성
    let r2 = &s; // 또 다른 불변 참조 생성
    // let r3 = &mut s; // 가변 참조 생성, 이 코드는 컴파일 오류를 발생시킵니다.

    println!("{} and {}", r1, r2);
    // println!("{}", r3); // r3는 유효하지 않습니다.
}
```

### 라이프타임(Lifetimes)

```rust
fn main() {
    let r;

    {
        let x = 5;
        r = &x;
    }

    // 여기서 x는 메모리 해제
```