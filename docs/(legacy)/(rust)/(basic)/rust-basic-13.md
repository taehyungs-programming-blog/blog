---
layout: default
title: "13. tuple"
parent: "(Rust 기초)"
grand_parent: "(Rust 🦀)"
great_grand_parent: "Legacy Archive"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```rust
let array = [1, 2, 3];
let tuple = (3, 5u32, 3.2);

let a : [i32;3] = [1, 2, 3];
let t : (i32, u32, f64) = (3, 5u32, 3.2);
```

```rust
// 요소 접근
let t : (i32, u32, f64) = (3, 5u32, 3.2);

println!("{}", t.0);
println!("{}", t.1);
println!("{}", t.2);
```

```rust
// 요소 변수로 받기
let (a, _, c) = t;  // _는 관심없다는 표현

println!("{}", a);
println!("{}", c);
```

```rust
// 함수가 한개 이상의 값을 반환하고 싶을때
fn return_two_value() -> (i32, f64)
{
    (0, 3.4)
}

fn main()
{
    let (first, second) = return_two_value();
}
```