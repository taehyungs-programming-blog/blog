---
layout: default
title: "02-00. 소소한 팁"
parent: "(Rust 기초)"
grand_parent: "(Rust 🦀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 주석

```rust
// comment
/* comment */
```

---

## 함수 생성시 주의사항

```rust
// fn을 적어 함수라 알려줘야 한다.
fn main()
{
    // ~~~
}
```

```rust
fn function_name() -> i32 // 반환을 할 경우 타입을 알려줘야 한다
{
    // ~~~
}
```

---

## 함수와 매크로 차이

```rust
foo();      // 얘는 함수
foo!();     // 얘는 메크로 - {매크로이름}! 이렇게 생겼음
```

* 그냥 함수 쓰면 안되나?
    * 함수 오버로딩, 가변 인자 등 문법을 지원하지 않아 매크로를 써야하는 케이스가 있다.
    * 이후에 설명!

---

## Naming Conversion

* snake case를 사용하며 규칙을 지키지 않을시 warning 발생
    * `shink_to_fit()` 이런식
* 단, 상수 이름은 모두 대문자로 한다

```rust
let count = 10;
let Count2 = 10;    // Warning 발생
```

---

## compile warning 제거하기

```rust
fn main()
{
    // 사용하지 않았다고 warning이 발생함.
    let v1 = 10;
}
```

```bash
$ rustc *.rc
```

```
warning: unused variable: `v1`
 --> .\hellorust.rs:4:9
  |
4 |     let v1 = 10;
  |         ^^ help: if this is intentional, prefix it with an underscore: `_v1`
  |
  = note: `#[warn(unused_variables)]` on by default

warning: 1 warning emitted
```

* 아래처럼할 경우 warning이 발생하지 않게 된다

```bash
$ rustc *.rc -A unused_variables
```

---

## bin파일 이름 변경해 빌드하기

```bash
$ rustc *.rc -o {newName}.exe
```