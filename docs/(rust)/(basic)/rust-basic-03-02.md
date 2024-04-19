---
layout: default
title: "03-02. 소유권(Ownership) 이란?"
parent: "(Rust 기초)"
grand_parent: "(Rust 🦀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Reference ✨](https://rinthel.github.io/rust-lang-book-ko/ch04-00-understanding-ownership.html)

* 소유권(Ownership)은 러스트의 가장 유니크한 특성이며, **러스트가 가비지 콜렉터 없이 메모리 안정성 보장**을 하게 해줍니다. 
* 그러므로, 소유권이 러스트 내에서 어떻게 동작하는지 이해하는 것은 중요합니다. 

---

## OwnerShip 규칙

* 아래는 규칙이니 받아 들이고 들어가자
    * 러스트의 각각의 값은 해당값의 오너(owner)라고 불리우는 변수를 갖고 있다.
    * 한번에 딱 하나의 오너만 존재할 수 있다.
    * 오너가 스코프 밖으로 벗어나는 때, 값은 버려진다(dropped).

---

## Stack, Heap 복사 방법

```rust
let s1 = String::from("hello");
let s2 = s1.clone();

println!("s1 = {}, s2 = {}", s1, s2);
```

```rust
let x = 5;

// stack은 이렇게만 해도 복사가 일어남.
let y = x;

println!("x = {}, y = {}", x, y);
```

* 복사가 되는 타입들
    * u32와 같은 모든 정수형 타입들
    * true와 false값을 갖는 부울린 타입 bool
    * f64와 같은 모든 부동 소수점 타입들
    * Copy가 가능한 타입만으로 구성된 튜플들. (i32, i32)는 Copy가 되지만, (i32, String)은 안됩니다.

---

## 소유권과 함수사용시 주의점

```rust
fn main() {
    let s = String::from("hello");  // s가 스코프 안으로 들어왔습니다.

    takes_ownership(s);             // s의 값이 함수 안으로 이동했습니다...
                                    // ... 그리고 이제 더이상 유효하지 않습니다.
    let x = 5;                      // x가 스코프 안으로 들어왔습니다.

    makes_copy(x);                  // x가 함수 안으로 이동했습니다만,
                                    // i32는 Copy가 되므로, x를 이후에 계속
                                    // 사용해도 됩니다.

} // 여기서 x는 스코프 밖으로 나가고, s도 그 후 나갑니다. 하지만 s는 이미 이동되었으므로,
  // 별다른 일이 발생하지 않습니다.

fn takes_ownership(some_string: String) { // some_string이 스코프 안으로 들어왔습니다.
    println!("{}", some_string);
} // 여기서 some_string이 스코프 밖으로 벗어났고 `drop`이 호출됩니다. 메모리는
  // 해제되었습니다.

fn makes_copy(some_integer: i32) { // some_integer이 스코프 안으로 들어왔습니다.
    println!("{}", some_integer);
} // 여기서 some_integer가 스코프 밖으로 벗어났습니다. 별다른 일은 발생하지 않습니다.
```

* 해결책?

```rust
fn main() {
    let s1 = gives_ownership();         // gives_ownership은 반환값을 s1에게
                                        // 이동시킵니다.

    let s2 = String::from("hello");     // s2가 스코프 안에 들어왔습니다.

    let s3 = takes_and_gives_back(s2);  // s2는 takes_and_gives_back 안으로
                                        // 이동되었고, 이 함수가 반환값을 s3으로도
                                        // 이동시켰습니다.

} // 여기서 s3는 스코프 밖으로 벗어났으며 drop이 호출됩니다. s2는 스코프 밖으로
  // 벗어났지만 이동되었으므로 아무 일도 일어나지 않습니다. s1은 스코프 밖으로
  // 벗어나서 drop이 호출됩니다.

fn gives_ownership() -> String {             // gives_ownership 함수가 반환 값을
                                             // 호출한 쪽으로 이동시킵니다.

    let some_string = String::from("hello"); // some_string이 스코프 안에 들어왔습니다.

    some_string                              // some_string이 반환되고, 호출한 쪽의
                                             // 함수로 이동됩니다.
}

// takes_and_gives_back 함수는 String을 하나 받아서 다른 하나를 반환합니다.
fn takes_and_gives_back(a_string: String) -> String { // a_string이 스코프
                                                      // 안으로 들어왔습니다.

    a_string  // a_string은 반환되고, 호출한 쪽의 함수로 이동됩니다.
}
```

* 참조로 해결이 가능하다. 다음장에서 설명.