---
layout: default
title: "14. vector"
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

* array Vs vector
    * **array** - 요소 선언 가능(추가, 삭제, 크기 변경 불가능)
    * **vector** - 요소 선언, 추가, 삭제, 크기 변경 가능

```rust
fn main()
{
	let mut a = [1,2,3];        // stack에 올리감
	let mut v = vec![1,2,3];    // heap에 올라감
	
	// 1 요소 변경 - 배열, vector 모두 가능
	a[0] = 0;  // ok
	v[0] = 0;  // ok

	// 2 항목의 추가
//	a.push(20); // error
	v.push(20); // ok

	// 3 크기 변경
//	a.resize(10, 0); // error
	v.resize(10, 0); // ok

	println!("{:?}", v);

	println!("{:?}", std::mem::size_of_val(&a));
	println!("{:?}", std::mem::size_of_val(&v));

	println!("{:p}      ", &a);
	println!("{:p}, {:p}", &v, v.as_ptr());	
}
```

```rust
fn main()
{
	// vector 를 생성하는 방법
	let v1 : std::vec::Vec<i32> = Vec::new();
	let v2 : Vec<i32> = Vec::new();
//	let v3  		  = Vec::new(); // error
	let v4            = Vec::<i32>::new();
	let v5            = Vec::from([1,2,3]);
//	let v6 : Vec<i32> = [1,2,3].into();
	let v6 : Vec<_> = [1,2,3].into();


	// vec! 매크로 사용
	let     v7 = vec![5, 6, 7];
	let mut v8 = vec![5, 6, 7];	

}
```

```rust
fn main()
{
	// method
	let mut v = vec![1,2,3];

	v.push(4);
	v.extend([5,6]);

	println!("{:?}", v); // [1,2,3,4,5,6]

	// pop() method
//	v.clear();
	let ret = v.pop();

	println!("{:?}", v);  // [1,2,3,4,5]
	println!("{:?}", ret);// Some(6)으로 pop된다.
    // pop의 실패(None)가능성 때문이다.

    // 숫자만 꺼내는 방법
	let value = match v.pop()
			 {
				None => -1, 
				Some(n) => n,
			 };

	println!("{}", value );
}
```

```rust
fn check( v: &Vec<i32>)
{
	println!("{}, {}, {:p}", v.len(), 
			v.capacity(), v.as_ptr());
}

fn main()
{
	let mut v = vec![1,2,3,4,5];

	check(&v); // 5, 5, 

	v.resize(3, 0);

	check(&v); // 3, 5, 

	v.push(4); // 끝에 추가, 빠르게 동작

	check(&v); // 4, 5, 

	v.shrink_to_fit();

	check(&v); // 4, 4, 

	v.push(5); // 끝에 추가, capaciy==len
			   // 메모리 재할당 필요
			   // 성능 좋지 않다.
	check(&v); // 4, 8 
}
```

```rust
fn main()
{
	let a1 = [1,2,3];
	let a2 = a1;

	let v1 = vec![1,2,3];
	let v2 = v1;            // move!
//	let v2 = v1.clone();    // copy

	println!("{:?}", v2);
	println!("{:?}", v1); // error
}
```