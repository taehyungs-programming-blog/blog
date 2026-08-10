---
layout: default
title: "26. reference safety"
parent: "(Rust 기초)"
grand_parent: "(Rust 🦀)"
great_grand_parent: "Legacy Archive"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```rust
// (규칙1) rust는 Null reference를 만들수 없다.
// (규칙2) reference는 소유자의 수명 보다 오래 사용될 수 없다

fn main()
{
	let n = 10;

	{
		let r = &n;

		println!("{}", r);	
	}
	
    // 이건 가능
	println!("{}", n);
}
```

```rust
fn main()
{
	let r;

	{
		let n = 10;
		r = &n;
	} 

// 이건 안됨
//	println!("{}", r);
}
```

```rust
// (규칙3) immutable reference는 동시에 몇개라도 만들수 있다

fn main()
{
	let n = 10;
	let r1 = &n;
	let r2 = &n;

	println!("{}", n);
	println!("{}", r1);
	println!("{}", r2);
	println!("{}", n);
}
```

```rust
// (규칙4) mutable reference는 만들어 사용할 경우 소유자도 사용할 수 없다

fn main()
{
	let mut n = 10;

	let r1 = &mut n; // <= 대여 시작

	n = 20;         // 불가능
	
	*r1 = 20;		// <= 반납	
	
//	n = 20;         // 가능
}
```

```rust
// (규칙5) mutable refence를 만들어 사용중인 경우, 또 다른 refence를 만들수 없다

fn main()
{
	let mut n = 10;

	let r1 = &mut n;  // <== 대여 시작

//	let r2 = &n;        // 불가능

	println!("{}", r1); // <== 반납
	
	let r2 = &n;
}
```