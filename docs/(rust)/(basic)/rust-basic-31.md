---
layout: default
title: "31. struct"
parent: "(Rust 기초)"
grand_parent: "(Rust 🦀)"
nav_order: 4
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## struct

```rust
struct MyStruct1
{
	field1 : i32,
	field2 : f64,
}

// 필드(데이터)의 이름이 없는 struct도 가능
struct MyStruct2(i32, f64);

// 필드(데이터)가 하나도 없는 struct도 가능
    // unit-like struct
struct MyStruct3;

fn main()
{
}
```

```rust
struct Point
{
	x : i32,
	y : i32,	
}

fn main()
{
	let pt1 = Point{x:10, y:20};            // stack에 쌓인다
	let pt2 = Box::new( Point{x:10, y:20} );// Heap에 쌓인다(Box:: 스마트 포인터)
	
	print_type_of(&pt1); // Point
	print_type_of(&pt2); // Box<Point>

	println!("{}", std::mem::size_of_val(&pt1));
	println!("{}", std::mem::size_of_val(&pt2));
}


fn print_type_of<T>(_: &T) 
{
    println!("{}", std::any::type_name::<T>())
}
```

```rust
struct Point
{
	x : i32,
	y : i32,
}

fn main()
{
	let x = 5;
	let y = 3;

	let pt1 = Point{x:10, y:20};
	let pt2 = Point{y:20, x:10}; // ok
//	let pt3 = Point{x:10};		 // error
	let pt4 = Point{x, y};		 // ok
	let pt5 = Point{x:30, ..pt1};// ok
	let pt6 = Point{y:30, ..pt1};// ok

	let mut pt7 = Point{x:10, y:20};
//	pt1.x = 30; // error
	pt7.x = 30; // ok


	let x1 = pt1.x;
	let y1 = pt1.x;

    // x항목을 x2에 y항목을 y2에 넣어달라
	let Point{x:x2, y:y2} = pt1;

	let Point{x, y} = pt2;
	
	println!("{}, {}", x2, y2); // 10, 20
	println!("{}, {}", x, y);	// 10, 20
}
```

```rust
struct Color(u8, u8, u8);
struct Point(i32, i32);

fn main()
{
	let c1 = Color(255, 0, 0);
	let c2 = Color(0, 255, 0);

	let red = c1.0;

	println!("{red}");

	let Color(r, g, b) = c1;

	println!("{r}, {g}, {b}");
}
```

---

## struct method

```rust
struct Rect
{
	left   : u32, 
	top    : u32, 
	right  : u32, 
	bottom : u32,
}

impl Rect
{
	fn get_area( &self ) -> u32
	{
		(self.right-self.left) * 
			( self.bottom - self.top)
	}
}

fn main()
{
	let rc = Rect{left:1,   top:1, 
				  right:10, bottom:10};

//	let area = rc.get_area();
			// Rect::get_area(&rc)
	let area = Rect::get_area(&rc);


	println!("{}", area);
}
```

```rust
struct Rect
{
	left   : u32, 
	top    : u32, 
	right  : u32, 
	bottom : u32,
}

impl Rect
{
	fn get_area( &self ) -> u32
	{
		(self.right - self.left) * (self.bottom - self.top)
	}
	fn inflate( &mut self, dx : u32, dy : u32 ) 
	{
		self.right += dx;
		self.bottom += dy;
	}
}

fn main()
{
	let mut rc = Rect{left:1,top:1, right:10, bottom:10};

	rc.inflate(10, 10);
		// Rect::inflate(&mut rc, 10, 10 );

	println!("{}, {}, {}, {}", rc.left, rc.top, rc.right, rc.bottom);
}
```

```rust
struct Rect
{
	left   : u32, 
	top    : u32, 
	right  : u32, 
	bottom : u32,
}

impl Rect
{
	fn get_area( &self ) -> u32
	{
		(self.right - self.left) * (self.bottom - self.top)
	}

	fn inflate( &mut self, dx : u32, dy : u32 ) 
	{
		self.right += dx;
		self.bottom += dy;
	}

	fn new( left : u32, top : u32, right : u32, bottom: u32 ) -> Self 
	{
		let rc = Rect{left, top, right, bottom};

		rc
	}
}

fn main()
{
//	let rc = Rect{left:1,top:1, right:10, bottom:10};
	let rc = Rect::new(1,1,10,10);

	println!("{}, {}, {}, {}", rc.left, rc.top, rc.right, rc.bottom);
}
```