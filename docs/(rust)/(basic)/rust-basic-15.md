---
layout: default
title: "15. string"
parent: "(Rust 기초)"
grand_parent: "(Rust 🦀)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* rust의 문자열은 두 가지가 있다.
    * `&str` - 문자열 자원을 소유하지 않음(C++의 std::string_view)
    * `String` - 문자열 자원을 소유(C++의 std::string) <- 지금은 이거만 확인할 예정

```rust
fn main()
{
	let s1 = "ABCD";			   // &str 타입	   
	let s2 = String::from("ABCD"); // String 타입


	println!("{}, {}", s2.len(), s2.capacity());
					// 4, 4

	println!("{}", std::mem::size_of_val(&s2));
					// 24


	println!("{:p}", &s2);          // s2자체의 주소
	println!("{:p}", s2.as_ptr());  // string이 저장된 heap의 주소
}
```

```rust
fn main()
{
	// string 객체를 생성하는 방법
	let s1 = String::new();
	let s2 = String::from("ABCD");
	let s3 = "ABCD".to_string();
	
	// method
	println!("{}, {}", s1.len(),      s2.len());  		// 0, 4
	println!("{}, {}", s1.is_empty(), s2.is_empty());	// true, false
	println!("{}, {}", s2.starts_with("AB"), s2.ends_with("AB")); // true, false

	// 문자(열) 추가, 변경
	let s4     = "ABCD".to_string();
	let mut s5 = "ABCD".to_string();

//	s4.push('X');	// error
	s5.push('X');
	s5.push_str("OPQ");

	println!("{}", s5); // ABCDXOPQ

	let s6 = s5.replace("BCD", "-----");
	println!("{}", s6); // A-----XOPQ

	// 검색
	let ret = s5.find("CD"); // Option<T>

	println!("{:?}", ret); // Some(2)
}
```

```rust
fn main()
{
	// Capacicy 개념
	let mut s = "ABCD".to_string();

	println!("{}, {}", s.len(), 		// 4
				       s.capacity());	// 4

	s.push('E');

	println!("{}, {}", s.len(), 		// 5
				       s.capacity());	// 8

	// 복사(대입)은 move 사용

	let s1 = "ABCD".to_string();
//	let s2 = s1;
	let s2 = s1.clone();

	println!("{}", s2); // "ABCD"
	println!("{}", s1); // error

}
```