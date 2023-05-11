---
layout: default
title: "30. iterator"
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
	let v = vec![1,2,3,4,5];

//	let it = v.iter();
	let mut it = v.iter();
    // Some으로 출력됨을 주의
//	while let Some(e) = it.next() // e &i32
	for e in v					  // e i32
	{
		println!("{}", e);
	}


	/*
	println!("{:?}", it.next());
	println!("{:?}", it.next());
	println!("{:?}", it.next());
	println!("{:?}", it.next());
	println!("{:?}", it.next());
	println!("{:?}", it.next());
	println!("{:?}", it.next());
	*/
}
```

```rust
fn main()
{
	let v = vec![1,2,3,4,5,6,7,8,9,10];

	let it1 = v.iter();
	let it2 = it1.rev();        // 거꾸로 출력
	let mut it3 = it2.map(|e| e * 2);

    // 위와 동일한 표현
	let mut it4 = v.iter().rev().map(|e| e * 2);

	while let Some(e) = it4.next()
	{
		println!("{:?}", e);
	}

	let v1 = vec![1,2,3,4,5,6,7,8,9,10];

    // iterator를 수집해 vector로 만들어 달라
	let v2 = v.iter().rev().map(|e| e * 2).collect::<Vec<i32>>();
	println!("{:?}", v2);
}
```

```rust
fn main()
{
	let mut v = vec![1,2,3,4,5];

//	let mut it = v.iter();
	let mut it = v.iter_mut();

	while let Some(e) = it.next()
	{
		*e = 0;
	}

	println!("{:?}", v); 
}
```

```rust
fn main()
{
	let mut v1 = vec![1,2,3,4,5];
	let mut v2 = vec![1,2,3,4,5];

	let mut it1 = v1.iter();
	let mut it2 = v2.iter_mut();

	let it1_e = it1.next();
	let it2_e = it2.next();

	let Some(e1) = it1_e else { todo!() };
	let Some(e2) = it2_e else { todo!() };

//	*e1 = 0; // error. &i32
	*e2 = 0; // ok.    &mut i32

	println!("{:?}", v2);
}
```

```rust
fn main()
{
	let mut v1 = vec![1,2,3,4,5];
	let mut v2 = vec![1,2,3,4,5];

	let mut it1 = v1.iter();
	let mut it2 = v2.iter_mut();

	print_type_of(&it1);
	print_type_of(&it2);

	let it1_e = it1.next();
	let it2_e = it2.next();

	print_type_of(&it1_e);
	print_type_of(&it2_e);

	let Some(e1) = it1_e else { todo!() };
	let Some(e2) = it2_e else { todo!() };

	print_type_of(&e1);
	print_type_of(&e2);
}

fn print_type_of<T>(_:&T)
{
	println!("{}", std::any::type_name::<T>());
}
```

* 더 다양한 iterator

```rust
fn main()
{
	let v1 = vec![1,2,3,4,5];
	let mut v2 = vec![1,2,3,4,5];
	let v3 = vec![1,2,3,4,5];

	let mut it1 = v1.iter();	
	let mut it2 = v2.iter_mut();
	let mut it3 = v3.into_iter(); 

//	println!("{:?}", v3);

	let v1 = it1.next();
	let v2 = it2.next();
	let v3 = it3.next();

	print_type_of(&v1);
	print_type_of(&v2);
	print_type_of(&v3);
}

fn print_type_of<T>(_:&T)
{
	println!("{}", std::any::type_name::<T>());
}
```

```rust
fn main()
{
	let v1 = vec![1,2,3,4,5];
//	let v2 = v1.iter()
	let v2 = v1.into_iter()
	           .map(|e| e * 2)
			   .collect::<Vec<i32>>();

	println!("{:?}", v1);
	println!("{:?}", v2);
}
```

```rust
fn main()
{
	let     v1 = vec![1];
	let     v2 = vec![1];
	let mut v3 = vec![1];
	for e in v1       { print_type_of(&e); }
	for e in &v2      { print_type_of(&e); }
	for e in &mut v3  { print_type_of(&e); }

//	println!("{:?}", v1);
	println!("{:?}", v2);
	println!("{:?}", v3);	
}


fn print_type_of<T>(_:&T)
{
	println!("{}", std::any::type_name::<T>());
}
```