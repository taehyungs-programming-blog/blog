---
layout: default
title: "12. array"
parent: "(Rust 기초)"
grand_parent: "(Rust 🦀)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```rust
fn main()
{
	// 1 array basic - 모두 stack에 올라간다
	let arr = [1,2,3,4,5];

	println!("{}", arr[1]);

//	println!("{}", arr);	// error
	println!("{:?}", arr);	// ok

	// 2 mutable vs immutable
	let x1 = [1,2,3,4,5];
	let mut x2 = [1,2,3,5,6];

//	x1[0] = 10; // error	
	x2[0] = 10;	// ok
}
```

```rust
fn main()
{
	// 1 array 타입
	let arr1 : [i32;5] = [1,2,3,4,5];
	let arr2 : [f64;3] = [1.1, 2.2, 3.3];
	let arr3 = [1,2,3,4,5];
	
	// 2 array 크기(반드시 컴파일 타임에 알아야함.), 메모리 크기
	let sz = 5;
//	let arr4 : [i32;sz] = [1,2,3,4,5]; // error	

	println!("{}", arr3.len());  // 5
	println!("{}", std::mem::size_of_val(&arr3));
	

	// 3 array 를 생성하는 방법

	let good1 : [i32;5] = [1,2,3,4,5];
	let good2           = [1,2,3,4,5];
	let good3 		    = [0;5];	

//	let bad1 : [i32;5];		// 사용시 에러
//	let bad2 : [i32;5] = [1,2,3]; 		// error
//	let bad3 : [i32;5] = [1,2,3,4,5,6]; // error	
}
```

```rust
fn twice(n : i32) -> i32 
{
	 return n * 2;
}

fn main()
{
	// 1 [] vs get() method
	println!("{}", x1[1]);      // 2
	println!("{:?}", x1.get(1));// Some(2) 
	println!("{:?}", x1.get(5));// None 

	// 2 map
	let x1 = [1,2,3];
//	let x2 = x1.map(인자가 한개인 함수);
//	let x2 = x1.map(twice);
	let x2 = x1.map(|n| n * 2 );

	println!("{:?}", x2); // 2, 4, 6	

}
```

```rust
fn main()
{	
	// 1 array 과 복사 가능
	let x1 = [1,2,3,4,5];
	let x2 = x1;	

	let mut x3 = [0;5];
	x3 = x2;	

	// 2 타입이 다른 경우는 복사(대입) 안됨
//	let mut x4 = [0, 0, 0]; 
//	x4 = x2;	// error

	f1(x1);
	f2(&x1);
}

// call by value
fn f1( x : [i32;5]) {}

// call by reference
fn f2( x : &[i32;5]) {}
```