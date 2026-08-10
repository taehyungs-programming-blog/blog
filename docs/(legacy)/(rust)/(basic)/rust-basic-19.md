---
layout: default
title: "19. generic"
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

* C++로 말하면 Template

```rust
fn is_equal<T>(a : T, b : T)
	where T : std::cmp::PartialEq/*==이 가능한가*/ + std::fmt::Display /*{}로 출력이 가능한가*/
{
	let ret = a == b;

	println!("{} == {} is {}", a, b, ret);
}

fn main()
{
	is_equal(2, 3);
	is_equal(3, 3);
	is_equal(3.2, 3.2);

}
```

```rust
fn size_of<T>()
{
	println!("{}", std::mem::size_of::<T>());	
}

fn main()
{
//	size_of();
//	size_of<i32>(); // C#, C++
	size_of::<i32>();
}
```

```rust
fn log<T, U>(a : T, b : U)
	where T : std::fmt::Debug, 
		  U : std::fmt::Debug,
{
	println!("{:?}, {:?}", a, b);
}

fn main()
{
	log(3, 3.4);
}
```

```rust
struct MyType1
{
	x : i32,
	y : i32,
}
struct MyType2<T>
{
	x : T,
	y : T,
}
fn main()
{
	let t : MyType1      = MyType1{x:10, y:20};

	let t : MyType2<i32> = MyType2::<i32>{x:10, y:20};
}
```