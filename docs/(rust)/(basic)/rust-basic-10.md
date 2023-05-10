---
layout: default
title: "10. type inference"
parent: "(Rust 기초)"
grand_parent: "(Rust 🦀)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## type inference

* 한국말로 타입추론

```rust
fn print_type_of<T>(_: &T) 
{
    println!("{}", std::any::type_name::<T>())
}

fn main()
{
    // 정수는 기본적으로 i32
    // 실수는 기본적으로 f64
	let n = 3;
	let f = 3.4;

	print_type_of(&3);      // i32
	print_type_of(&3.4);    // f64
	print_type_of(&n);      // i32
	print_type_of(&f);      // f64
}
```

```rust
// 캐스팅에 엄격하다
fn main()
{
	let n1 : i32 = 10;
	let n2       = 10;

//	let a1 : u32 = n1;		// error
	let a2 : u32 = 10u32;	// ok
	let a3 : u32 = 10;		// ok - 이건 사용성을 위해 error를 리턴하지 않음
	let a4 : u32 = n2;		// ok - 여기부터 n2도 u32로 추론함

	let a5 : i32 = n2;      // Error - a4에서 n2를 u32로 추론햇기에 i32로 캐스팅 불가
}
```

```rust
fn main()
{
	let n1 : i32 = 0;
	let n2       = 0;

	let ret1 = n1.max(20);
	let ret2 = n2.max(20);
	
	let ret3 = n1.count_zeros();
	let ret4 = n2.count_zeros();    // Error - n의 type을 알수 없음.(현재 추론안됨)
}
```

```rust
fn main()
{
	let n1 : i32 = 10;
	let n2 = 10;


	let n3 : i32;
	let n4;

//	n3 = 10u32;	// error
	n4 = 10u32; // 타입을 명시 하지 않았기에 가능
}
```