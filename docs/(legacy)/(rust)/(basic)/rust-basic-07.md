---
layout: default
title: "07. variable"
parent: "(Rust 기초)"
grand_parent: "(Rust 🦀)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 변수선언 기본

```rust
fn main()
{
	// 1 변수를 선언하는 2가지 방법
	let v1 : i32 = 10;	// type annotation 을 표기
	let v2       = 10;  // type annotation 을 생략
    /*
    * 부호 있는 정수 - i8, i16 ~
    * 부호 없는 정수 - u8, u16 ~
    * 실수 - f32, f64
    */

	// 2 bool
	let b1 = true;
	let b2 = false;

	// 3 primitive 타입 외에도 표준 라이브러리 안에 
	//   다양한 타입 제공
	let s : String   = String::new();
	let v : Vec<i32> = Vec::new();		

    // 4 초기화 되지 않은 변수는 사용할수 없다
    let v1 : i32;
    println!("{}", v1); // error
}
```

---

## rust literal 표기법

```rust

fn main()
{	
	// 1 binary, octal, dec, hex 
	let n1 = 10;	// 10진수
	let n2 = 0b10;	// 2진수
	let n3 = 0o10;	// 8진수
	let n4 = 0x10;	// 16진수

	// 2 literal suffix
	let v1       = 10;	// i32 type
	let v2 : u32 = 10;	// u32 type
	
        // 이런식으로 타입지정가능
	let v3 = 10u32;		// u32 type
	let v4 = 10_u32;	// u32 type
	let v5 = 3.4;		// f64 type
	let v6 = 3.4f32;	// f32 type
	let v7 = 0x10u8;	// u8  type 

	// 3 digit separater
	let n6 = 1_000_000;
}
```

---

## mutable Vs immutable

```rust
fn main()
{
	// 1 mutable vs immutable
	let     v1 = 10; // immutable, read 만 가능
	let mut v2 = 10; // mutable,   r/w 모두 가능


	v1 = 20; // error
	v2 = 20; // ok.


	// 2 mut 키워드는 변수명 앞에만 표기 
	// type annotation 에는 표기 하지 않음. 
	// ( 단, reference 를 만들 때는 타입에 표기 )
	let mut v3	     = 10;
	let mut v4 : i32 = 10;	
    // let mut v4 : mut i32 = 10;	// 이런거 하지말란 말
}
```

---

## 변수의 다양한 정보 구하기

```rust
fn main()
{
	let v1 = 10;

	// 1 변수의 주소 출력
	println!("{}", v1);     // 10
	println!("{}", &v1);    // 10 - v1의 reference인데 그냥 출력하면 값이 나옴
	println!("{:p}", &v1);


	// 2 변수의 메모리 크기
	println!("{}", std::mem::size_of_val(&v1)); //4
	println!("{}", std::mem::size_of::<i32>());


	// 3 타입 이름 조사
	let v2 = 10;		// i32
	let v3 = 10u32;		// u32

	print_type_of(&v2);	
	print_type_of(&v3);	
}

// 일단은 이렇게 타입을 조사할수 있다 라 받아들이자.
fn print_type_of<T>(_: &T) 
{
    println!("{}", std::any::type_name::<T>())
}
```

---

## unit type

```rust
fn main()
{
    // () -> unit type
        // 없을을 의미하는 타입 혹은 값(empty tuple)
	let v1 : () = ();
	let v2      = ();

//	println!("{}", v1);		// error
	println!("{:?}", v1);
	println!("{:?}", v2);
}

fn f1() 
{	
}

fn f3() -> ()       // void 표기라 생각하자
{
}
```

---

## heap

```rust
fn main()
{
    let v1 = 10;            // stack
    let sp1 = Box::new(10); // heap

    // refernce
    let r1 : &i32 = &v1;
	let r2        = &v1;

    // pointer
    let p1 : *const i32 = &v1 as *const i32;
	let p2              = &v1 as *const i32;

    // 일단 지금은 reference, pointer를 선언할 수 있다 정로도 받아들이자.
}
```