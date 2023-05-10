---
layout: default
title: "05. println"
parent: "(Rust 기초)"
grand_parent: "(Rust 🦀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```rust
fn main()
{
	// 1 print! vs println!
	print!("hello");	// 문자열 출력후 개행 안됨
	println!("world");	// 문자열 출력후 개행


	// 2 프로그래밍에서 직접 개행 하려면 "\n" 사용
	print!("hello\n");	
	println!("hello\nworld");
}
```

```rust
fn main()
{	
	// 1 positional argument vs named argument	
	println!("{}, {}", 3, 1);				// 3, 1 
	println!("{0}, {1}, {0}", "A", 10);		// A, 10, A
	println!("{name}, {age}", name="john", age=20);	// john, 20


	// 2 변수값 출력 ( print variable )
	let v1 = 10;
	let v2 = 20;
	println!("{}, {}", v1, v2);			// 10, 20
	println!("{0}, {1}, {0}", v1, v2);	// 10, 20, 10
	println!("{v1}, {v2}");				// 10, 20


	// 3 주의 사항
	let arr = [1,2,3];
//	println!("{v1+v2}");	// error
//	println!("{arr[0]}")	// error
	println!("{}, {}", arr[0], v1 + v2); // 1, 30

	// 4 {{}} 표기법
	println!("{v1}");	// 10
	println!("{{v1}}");	// {v1}
}
```

```rust
fn main()
{
	// 1 formatting #1
	let v1 = 10;
	println!("{}", 	 v1); // 10		10진수
	println!("{:b}", v1); // 1010	2 진수
	println!("{:o}", v1); // 12		8 진수
	println!("{:x}", v1); // a		16진수, 소문자
	println!("{:X}", v1); // A		16진수, 대문자
	println!("{v1:X}");	  // 위와 동일	
	println!("============================");

	// 2 formatting #2
	println!("{}", 	v1);	// '10'
	println!("{:>6}", v1);	// '    10'		6자리, right  align
	println!("{:<6}", v1);	// '10    '		6자리, left   align
	println!("{:^6}", v1);	// '  10  '		6자리, center align
	println!("{:#>6}",v1);	// '####10'		
	println!("{:#<6}",v1);	// '10####'
	println!("{:#^6}",v1);	// '##10##'
	println!("{num:#>width$}", num=10, width=10);	// '#######10'		
	println!("============================");
	
	// 3 실수 정밀도
	let f1 = 3.141592;
	println!("{}", f1);		// 3.141592
	println!("{:.3}", f1);	// 3.142

	// 4 "standard library" 문서에서 "std::fmt" 문서 참고
}
```

```rust
fn main()
{
	let v1 = 10;

	// ❶ {}, {:?}, {:#?}
	println!("{}", v1);		// 10, 일반적인 출력
	println!("{:?}", v1);	// 10, 디버깅을 위한 출력
	println!("{:#?}", v1);  // 10, 디버깅을 위한 출력(for pretty print)


	// 2 배열은 {:?}, {:#?} 형태로만 출력 가능, {} 로 출력 안됨.
	let arr = [1,2,3];
	println!("{}", arr[0]); // 1
	println!("{}", arr);	// error (전체출력을 의도함)
	println!("{:?}", arr);  // 이건 디버깅용으로 전체 출력 가능
	println!("{:#?}", arr);	
}
```

```rust
fn main()
{
	// 1 string literal 만 출력 가능. 문자열 변수를 직접 출력할수는 없음
	let s = "hello";

	println!("hello");	// hello
	println!(s);		// error - println이 매크로임
	println!("{}", s);	// ok
	
	// 2 print vs eprint
	print!("hello");  	// stdout	
	println!("hello"); 

	eprint!("hello");  	// stderr
	eprintln!("hello"); 
}
```