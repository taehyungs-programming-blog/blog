---
layout: default
title: "24. move"
parent: "(Rust 기초)"
grand_parent: "(Rust 🦀)"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```cpp
#include <string>
using namespace std;

// C++ 코드
int main()
{
	string s1 = "to be or not to be";	
	string s2 = s1;

	string s3 = "to be or not to be";
	string s4 = move(s3);       // C++은 move를 명시해 줘야한다
}
```

```rust
fn main()
{
	let s1 = String::from("ABCD");
	let s2 = s1;                // rust는 별도의 명시가 없어도 자동 move

	let s3 = String::from("ABCD");
	let s4 = s3.clone();

//	println!("{}", s1); // error
	println!("{}", s3); // ok
}
```

```rust
fn main()
{
	let s1 = String::from("ABCD");
	let s2 = s1;

//	println!("{}", s1); // error


    // stack에 쌓이는 데이터는 자동복사가 일어난다
	let n1 = 10;
	let n2 = n1;

	println!("{}", n1); // ok
}
```

```rust
// 사용자 정의 타입은 어떨까?
#[derive(Copy, Clone)]
struct PointA {	x:i32, y:i32 }

struct PointB {	x:i32, y:i32 }

fn main()
{
	let pa1 = PointA{x:1, y:1};
	let pb1 = PointB{x:1, y:1};

	let pa2 = pa1; // copy
	let pb2 = pb1; // move

	println!("{}", pa1.x); // ok
//	println!("{}", pb1.x); // error
}
```

---

```rust
fn foo(x : i32, s : String)
{
}

fn main()
{
	let n = 10;
	let mut s = "ABC".to_string();
	
	foo(n, s);  // 이렇게 써버리면 s가 move되며 다시 s를 쓸 경우 에러발생
//	foo(n, s.clone());  // 이렇게 보내야 한다(너무 귀찮은디? 그래서 reference형태로 보내는게 좋음)

//	s = "ABC".to_string(); // 이런식으로 쓸수도 있는데 굳이...

	println!("{}", n);
	println!("{}", s);
}
```

---

```rust
fn main()
{
	let a = [1,2,3,4,5];        // copy type
	let v = vec![1,2,3,4,5];    // non-copy

//	for e in a
//	for e in v	// for(v) - 모두 move가 되어 error!
	for e in &v	// for(&v)
	{
	}

	println!("{:?}", a);
	println!("{:?}", v);
}
```

---

```rust
fn main()
{
	let s = "ABC".to_string();
	

	match s
	{
        x => println!("_"),         // move되며 error 발생
		// ref x => println!("_"),  // ref로 써야한다
	}

	println!("{}", s);
}
```