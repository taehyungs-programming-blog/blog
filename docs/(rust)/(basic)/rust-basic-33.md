---
layout: default
title: "33. user trait"
parent: "(Rust 기초)"
grand_parent: "(Rust 🦀)"
nav_order: 4
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```rust
struct Rect
{
	left  : u32, top    : u32, 
	right : u32, bottom : u32
}

trait Shape
{
	fn get_area(&self)->i32 
	{ 
		println!("Shape get_area"); 
		0
	}
	fn draw(&self);
}

impl Shape for Rect
{
	fn get_area(&self)->i32 
	{ 
		println!("Rect get_area"); 
		0
	}

	fn draw(&self){println!("draw Rect"); }
}

fn main()
{
	let r = Rect{left:1,   top : 1, 
				 right:10, bottom:10};
	r.draw();

	let ret = r.get_area();
	
}
```

```rust
struct MyType;

trait MyTrait
{
	type Output;

	fn method1(&self) {}
	fn method1(&self) -> Self::Output;
	fn afunction() {}
}

impl MyTrait for MyType
{
	type Output = i32;

	fn method1(&self) -> Self::Output
	{
		10
	}
}

fn main()
{
	
}
```

```rust
trait OddEven
{
	fn is_odd(&self)->bool;	
	fn is_even(&self)->bool { !self.is_odd() }
}

impl OddEven for i32
{
	fn is_odd(&self)->bool { self % 2 == 1 }
}

// error
impl std::ops::Drop for i32
{
	fn drop(&mut self) 
	{
		 println!("drop i32");
	}
}

fn main()
{
	let n = 10;

	println!("{}", n.is_odd());
	println!("{}", n.is_even());

	println!("{}", n);
}
```