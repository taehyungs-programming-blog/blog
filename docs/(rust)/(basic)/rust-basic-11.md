---
layout: default
title: "11.  expression"
parent: "(Rust 기초)"
grand_parent: "(Rust 🦀)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* **expression** - 하나의 값으로 계산되는 코드 집합, 핵심은 **리턴**으로 값을 뱉는다 이다.
    * 하나의 값을 생성
* **statement** - 블럭({})을 구성하는 요소, 세미 콜론으로 끝나는 코드 집합, 리턴이 None이다.
    * 표현식의 계산 후 그 결과를 무시
    * statement는 두 가지 로 나뉜다.
    * Declaration Statement - 블럭 안에서 한 개 이상의 이름을 알린다(변수 선언이라 생각하면 편하다)
    * Expression Statement - 표현식 뒤에 ;를 붙인것 (함수의 호출이라 생각하면 편하다)

```rust
fn foo() -> i32
{
	return 10;
}

fn main()
{
	let n1 = 3;
	let n2 = (n1 + 2) * foo();

	foo();

	let n3 = {3};	    // expression
	let n4 = {3;};      // ;를 붙임으로써 Statement가 된다. (리턴이 None)

	println!("{:?}", n3); // 3
	println!("{:?}", n4); // Void ()
}
```

```rust
fn f1() -> i32 
{
//	 return 10;
	 10 		// 위와 동일
//	 10;
}

fn main()
{
	let score = 30;

	let total = { let report = 30; 
				  score + report };
	
	println!("{:?}", total);        // 60

	println!("{:?}", f1() );        // 10
}
```

```rust
fn main()
{
	let score = 80;

//	let total = if score > 60 { 5 } else { 0 };

//	let total = if score > 60 { 5; } else { 0; };

//	let total = if score > 60 { 5; } else { 0 };

	let total = if score > 60 { 5 };

	println!("{:?}", total);
}
```

```rust
fn main()
{
	let score = 85;

	let total = match score
				{
					90..=100 => 'A',
					80..=89 => 'B',
					70..=79 => 'C',
					60..=69 => 'D',
					_ => 'E'
				};

	println!("{:?}", total);    // B
}
```