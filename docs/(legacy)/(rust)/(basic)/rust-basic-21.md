---
layout: default
title: "21. option"
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
fn find() -> i32
{
	-1	// (문제) -1이 실패란걸 모두가 알아야만 한다 ...
}

fn main()
{
	find();
}
```

```rust
enum Option<T>
{
	None,
	Some(T),
}

fn main()
{
    // 이런식으로 실패를 분리해 보자
	let opt1 : Option<i32> = Option::<i32>::None;
	let opt2 : Option<i32> = Option::<i32>::Some(30);
}
```

```rust
// 이미 Rust에서 Option을 지원함 아래처럼 그냥 쓰면 된다.

fn main()
{
	let opt1 : Option<i32> = Option::<i32>::None;
	let opt2 : Option<i32> = Option::<i32>::Some(30);

	let opt1  = Option::<i32>::None;
	let opt2  = Option::<i32>::Some(30);

	let opt1 : Option<i32> = None;
	let opt2 : Option<i32> = Some(30);

//	let opt1 = None;		// error
//	let opt2 = Some(30);	// error

	let ret = find();
}

fn find() -> Option<i32>
{
	None
}
```

```rust
fn main()
{
	let opt = Option::<i32>::Some(100);

	if opt == None 
	{
	}

	if opt == Some(100) 
	{
		
	}	
}
```

```rust
// Some의 값을 꺼내보자

fn main()
{
//	let opt = Option::<i32>::Some(100);
	let opt = Option::<i32>::None;

	let v1 = match opt 
			{
				Some(n) => n, 
				None	=> -1,
			};

	println!("{}", v1);


	let mut v2 = 10;

	match opt 
	{
		Some(n) => v2 = n,
		None    => println!("None"),
	}
	println!("{}", v2);
	
}
```

```rust
fn main()
{
	let opt = Option::<i32>::Some(100);
//	let opt = Option::<i32>::None;

	if let Some(n) = opt
	{
		println!("result : {}", n);
	}
	else
	{
		println!("None");
	}

	let Some(n) = opt else { todo!() };
	
}
```

```rust
fn find() -> Option<i32>
{
    // Option으로 실패/성공을 분리해보자
//	-1
//	None
	Some(5)
}

fn main()
{
	let ret = find();

	match ret
	{
		Some(n) => println!("{}", n),
		None    => println!("fail"),
	}

}
```

```rust
fn main()
{
	let s = String::from("ABCD");

	let ret = s.find('C');  // Some(2)가 나온다
//	let ret = s.find('X');

	println!("{:?}", ret);

	match ret
	{
		Some(n) => println!("{}", n),
        // 이런식으로 자연스럽게 에러처리를 유도할 수 있음
		None    => println!("fail"),
	}		

}
```