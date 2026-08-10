---
layout: default
title: "32. trait"
parent: "(Rust 기초)"
grand_parent: "(Rust 🦀)"
great_grand_parent: "Legacy Archive"
nav_order: 4
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```rust
struct Point
{
	x : i32,
	y : i32
}

impl std::fmt::Debug for Point 
{
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result 
	{
        f.debug_struct("== Point ==")
         .field("x", &self.x)
         .field("y", &self.y)
         .finish()
    }
}

fn main()
{
	let pt = Point{x:10, y:20};

    // 사용자 정의 타입을 {:?}로 출력하기 위해선 fmt의 정의가 필요하다
	println!("{:?}", pt);
}
```

```rust
// 좀 더 쉽게? -> Attributes 활용
#[derive(Debug)]
struct Point
{
	x : i32,
	y : i32
}

fn main()
{
	let pt = Point{x:10, y:20};

	println!("{:?}", pt);
}
```

```rust
struct Point 
{
    x: i32,
    y: i32,
}

// 나만의 형식으로 조금 더 쉽게 표현하고 싶다
impl std::fmt::Display for Point 
{
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result 
	{
        write!(f, "[({}, {})]", self.x, self.y)
    }
}

fn main()
{
	let pt = Point{x:10, y:20};

	println!("{}", pt);
}
```

---

```rust
struct Point
{
	x : i32,
	y : i32,
}

impl std::ops::Drop for Point
{
	fn drop(&mut self) 
	{
		 println!("drop Point");
	}
}

fn main()
{
	{ 
		let pt = Point{x:1, y:1};
		
		std::mem::drop(pt);
		
		println!("---------");	
	}
	println!("---------");
}
```

---

```rust
struct Point
{
	x : i32,
	y : i32,
}

impl std::default::Default for Point
{
	fn default() -> Self
	{
		Point{x:0, y:0}
	}
}

fn main()
{
	let mut v1 = vec![1,2,3];

//	v1.resize(10, 0);	
	v1.resize(10, i32::default());	

	let n1 = i32::default();

	let pt = Point::default();
}
```

---

* operator overloading

```rust
#[derive(Debug)]
struct Point
{
	x : i32,
	y : i32,
}

//impl std::ops::Add<i32> for Point
//impl std::ops::Add<Point> for Point
impl std::ops::Add for Point
{
	type Output = Point;

	fn add(self, p : Self ) -> Self::Output
	{
		Point{x:self.x + p.x, y:self.y + p.y  }
	}
}


fn main()
{
	let p1 = Point{x:1, y:1};
	let p2 = Point{x:2, y:2};

	let p3 = p1 + p2; // p1.add(p2)

	println!("{:?}", p3);
}
```

```rust
#[derive(Debug)]
struct Point
{
	x : i32,
	y : i32,
}

impl std::ops::Add<&Point> for Point
{
	type Output = Point;

	fn add(self, p : &Self ) -> Self::Output
	{
		Point{x:self.x + p.x, y:self.y + p.y  }
	}
}

impl<'a> std::ops::Add<&'a Point> for &'a Point
{
	type Output = Point;

	fn add(self, p : Self ) -> Self::Output
	{
		Point{x:self.x + p.x, y:self.y + p.y  }
	}
}


fn main()
{
	let p1 = Point{x:1, y:1};
	let p2 = Point{x:2, y:2};

//	let p3 = p1 + &p2; 	// p1.add(p2)
						// add(p1, p2)
	let p3 = &p1 + &p2;

	println!("{:?}", p3);
//	println!("{:?}", p1);
	println!("{:?}", p2);
}
```

```rust
fn main()
{
	let s1 = "AA".to_string();
	let s2 = "BB".to_string();

//	let s3 = s1 + s2;
	let s3 = s1 + &s2;

//	println!("{}", s1);
	println!("{}", s2);
}
```