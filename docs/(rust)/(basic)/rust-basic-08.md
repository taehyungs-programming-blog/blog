---
layout: default
title: "08. const, static"
parent: "(Rust 기초)"
grand_parent: "(Rust 🦀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## const

* 컴파일 시간 상수
* let대신 const 사용

```rust
fn main()
{
    let size0 = 10;

    const SIZE1 : u32 = 10;
    const SIZE2 = 10;           // error - 타입을 반드시 지정해야 함
    const size3 : u32 = 10;     // warning - 대문자로 만들어야 함
}
```

```rust
let mut n : i32 = 10;
const C :i32 = n;       // 컴파일 타임에 값을 알아야 해서 Error!
```

```rust
let size : usize = 10;
const SIZE : usize = 10;

// rust의 배열의 사이즈는 컴파일 타임에 알아야 한다
let arr1 : [i32;10];        // ok
let arr2 : [i32;size];      // error
let arr3 : [i32;SIZE];      // ok
```

---

## static

```rust
fn next_num() -> i32
{
    let mut cnt = 0;
    cnt = cnt + 1;
    return cnt;
}

fn main()
{
    println!("{}", next_num()); // 1
    println!("{}", next_num()); // 2 -> 이걸 원한다, 현재는 항상 1
}
```

```rust
// 이렇게 구현해야 한다
fn next_num() -> i32
{
    static mut CNT : i32 = 0;
    // static을 mutable 하려면 멀티 thread에 safe하지 않다는 표현을 해줘야한다
    // unsafe표현
    unsafe{CNT = CNT + 1};
    let n = unsafe{CNT};
    return n;
}
```

* 아니 전역변수를 그냥 쓰면 안되나?

```rust
let global_val = 10;        // Error

fn main(){
    // ~
}
```