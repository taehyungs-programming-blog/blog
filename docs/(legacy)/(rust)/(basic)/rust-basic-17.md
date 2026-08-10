---
layout: default
title: "17. while, loop, for, range"
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

## while

```rust
fn main()
{
	let mut i = 0;

	// 기본 모양
	while i < 10
	{
		println!("{}", i);
		i += 1; // ++i
	}

	// break, continue
	i = 0;
	while i < 10
	{
		i += 1;
		if i == 10 	  { break;}
		if i % 2 == 0 { continue;}
		println!("{}", i);
	}
}
```

```rust
fn main()
{
	let mut i = 0;
	let mut j = 0;

	'outer: while i < 10
	{
		j = 0;
		'inner: while j < 10
		{
			println!("{}, {}", i, j);
			j += 1;

			if j == 5
			{
                // 중첩된 while를 이렇게 탈출가능
                    // outer를 그냥 지정한 이름(내 마음대로 바꿔도 됨)
				break 'outer;
			}
		}
		i += 1;
	}
}
```

---

## loop

```rust
fn main()
{
	let mut i = 0;

//	while true 
	loop
	{
		i += 1;

		if i == 10
		{
			break;
		}
		println!("{}", i);
	}
}
```

* 그냥 `while true`가 낫지 않니?

```rust
fn main()
{
	let mut i = 0;
	let mut sum = 0;

    // loop는 표현식으로 처리가능하다.
	let value = loop 
//	let value = while true
				{
					i += 1;
					sum += i;
					if i == 10
					{
						break sum;
					}
				};

	println!("{:?}", value);
}
```

---

## for

```rust
fn main()
{
	let arr = [1,2,3,4,5];

	for e in arr
	{
		println!("{}", e);
	}

//	for e in 0..10  // 0 ~ 9
	for e in 0..=10 // 0 ~ 10
	{
		println!("{}", e);
	}
}
```

```rust
fn main()
{
	let mut arr = [1,2,3,4,5];

// 그냥 foreach를 써버리면 값변경이 안됨.
//	for e in arr
	for e in &mut arr
	{
		//e = 0;
		*e = 0;
	}
	println!("{:?}", arr);
}
```

---

## range

```rust
fn main()
{
    // 아래 둘은 같은 표현이다
	let r1 = 0..10;
	let r2 = std::ops::Range{start:0, 
							 end:10};

//	for e in 0..10
//	for e in r1
	for e in r2
	{
		print!("{}, ", e);
	}
}
```

```rust
fn main()
{
	let arr = [0,1,2,3,4,5,6,7,8,9];

//	for e in arr
//	for e in &arr[2..7]
    // 근데 &는 왜 붙이지?
    // 일단은 배열의 일부를 받기위해선 &를 붙여야한다 받아들이자
//	for e in &arr[2..]
//	for e in &arr[..7]
	for e in &arr[..]
	{
		print!("{}, ", e);
	}
}
```