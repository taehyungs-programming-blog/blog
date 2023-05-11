---
layout: default
title: "34. smart pointer"
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
fn main()
{
	let n1 = 10;
	let b1 = Box::new(10);

	let n2 = *b1;       // 10
	
	let r1 = &*b1;      // 10을 가리키는 참조

	println!("{:p} {}", b1, *b1);
	println!("{:p} {}", r1, *r1);
	println!("{}", std::mem::size_of_val(&b1));
	println!("{}", std::mem::size_of_val(&r1));
}
```

```rust
trait Draw   { fn draw(&self); }

struct Rect { x: i32}
struct Circle{ x: i32}

impl Draw for Rect   { fn draw(&self) { println!("draw Rect");} }
impl Draw for Circle { fn draw(&self) { println!("draw Circle");} }

fn main()
{
	let n1 = 10;
	let r1 : &i32     = &n1;
	let b1 : Box<i32> = Box::new(10);

	let rect = Rect{x:1};

	let r2 : &dyn Draw     = &rect;
//	let b2 : Box<dyn Draw> = Box::new(Rect{x:1});
	let b2 : Box<dyn Draw> = Box::new(Circle{x:1});

	println!("{}", std::mem::size_of_val(&b1));
	println!("{}", std::mem::size_of_val(&b2));
}
```

```rust
trait Draw   { fn draw(&self); }

struct Rect { x: i32}
struct Circle{ x: i32}

impl Draw for Rect   { fn draw(&self) { println!("draw Rect");} }
impl Draw for Circle { fn draw(&self) { println!("draw Circle");} }

fn main()
{

	let mut v : Vec<Box<dyn Draw>> = Vec::new();
	
	v.push( Box::new(Rect{x:1}));
	v.push( Box::new(Circle{x:1}));
	
	v[0].draw(); // 
	v[1].draw();
}
```

---

* rc - 자원을 공유하는 ptr

```rust
#[derive(Copy, Clone)]
struct Point
{
	x:i32, y:i32
}

fn main()
{
	let b1 = Box::new(Point{x:1, y:2});
	let b2 = b1;

	let b1 = Box::new(Point{x:1, y:2});
	let b2 = b1.clone();
	
	let b1 = Box::new(Point{x:1, y:2});
	let b2 = &*b1;
}
```

```rust
use std::rc::Rc;

#[derive(Copy, Clone, PartialEq)]
struct Point
{
	x:i32, y:i32
}

fn main()
{
	let rc1 = Rc::new(Point{x:1, y:2});
	let rc2 = rc1;
//	println!("{}", rc1.x); // error

	let rc1 = Rc::new(Point{x:1, y:2});
	let rc2 = rc1.clone();
	println!("{}", rc1.x);

	println!("{}", Rc::strong_count(&rc1));
	println!("{}", Rc::strong_count(&rc2));
	println!("{}", rc1.eq(&rc2));

	println!("{:p}", &*rc1);
	println!("{:p}", &*rc2);

}
```