---
layout: default
title: "04. attribute"
parent: "(Rust 기초)"
grand_parent: "(Rust 🦀)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* `#![]` or `#[]` 요렇게 생겼고
* 컴파일러에게 정보를 전달하기 위한 목적으로 기입한다

```rust
#![allow(dead_code)]
#![allow(unused_variables)] // 미사용 변수의 warning을 제거해 달라 (-A unused_variables)

#[derive(Debug)]
struct Point
{
	x : i32,
	y : i32,
}

fn main()
{
	
}
```

```rust
fn main()
{
    // 이렇게도 쓰인다
    #[allow(unused_variables)]
    let v1 = 10;
}
```

```rust
fn main()
{
    #[allow(unused_variables)] // - outter attributes라 아래 문장만 적용이 됨
    let v1 = 10;
    let v2 = 10;        // v2 경고가 난다
}

// #![allow(unused_variables)] - inner attribute로 자신의 스코프내에 모두 적용이 됨
```

```rust
struct Point
{
	x:i32,
	y:i32,
}

fn main()
{
	let pt1 = Point{x:10, y:20};
	let pt2 = Point{x:10, y:20};

	match pt1 == pt2    // 여기서 에러가 발생 ==를 match에서 쓸수없다
	{
		true  => println!("same"),
		false => println!("not same"),
	}
}
```

* `#[derive(PartialEq)]`로 해결가능

```rust
#[derive(PartialEq)]

struct Point
{
	x:i32,
	y:i32,
}

fn main()
{
	let pt1 = Point{x:10, y:20};
	let pt2 = Point{x:10, y:20};

	match pt1 == pt2
	{
		true  => println!("same"),
		false => println!("not same"),
	}
}
```