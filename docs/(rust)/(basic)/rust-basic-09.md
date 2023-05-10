---
layout: default
title: "09. method"
parent: "(Rust 기초)"
grand_parent: "(Rust 🦀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## primitive type과 메소드

* C++, Java는 멤버함수(메소드)가 없다.
    * `int n = 0; n.start();` 이런게 불가능
* C#, Swift ... primitive type도 멤버함수(메소드)가 있다.
* Rust도 있다!

```rust
fn main()
{
	let v1 : i32 = 19;

	// 1  primitive 타입의 변수도 메소드가 있다
	println!("{0}, {0:x}, {0:b}", v1);
	println!("{}", v1.count_ones()); // 3
	println!("{}", v1.count_zeros());// 32-3
	println!("{}", v1.is_negative());
	println!("{}", v1.pow(2));	
						// 19^2 => 19 * 19

	// 2 주의 사항 - type을 지정해 줘야함.
	let v2 = 19;
	println!("{}", v2.pow(2));	// error					


	// 3 literal 도 메소드 호출가능
	println!("{}", 10_i32.pow(3));
	println!("{}", 10.pow(3));	// error - 역시 타임을 알려줘야 한다


	// 4 associated function 
	println!("{}", u32::min_value() );
	println!("{}", u32::max_value() );
				// C#, java :  u32.min_value()
				// C++      :  u32::min_value()
}
```

---

## Casting

```rust
fn main()
{
	let n1 = 10;

	// 1 암시적 변환 허용 안됨
	let f1 : f64 = n1;  // Error


	// 2 명시적 변환
	let f2 = n1 as f64;
	let f3 = f64::from(n1);
	
	let n3 = 3.4 as i32;
	
	// 3 문자열 => 정수 또는 실수
	let a = "10".parse::<i32>().unwrap();
	let b = "3.4".parse::<f64>().unwrap();

	
	println!("{:?}", a);
	println!("{:?}", b);	
}
```