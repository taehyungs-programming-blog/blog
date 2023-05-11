---
layout: default
title: "18. function"
parent: "(Rust 기초)"
grand_parent: "(Rust 🦀)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 함수

```rust
fn add1(x : i32, y : i32) -> i32 
{
	return x + y;
}

fn add2(x : i32, y : i32) -> i32 
{
    // 표현식(;없음)으로 표현시 자동 return이라 받아들이자
	x + y
}

fn no_return1()      
{	
}

fn no_return2() -> () 
{
}

fn no_return3() 
{
//	()
//	return ();
}

fn main()
{
}
```

```rust
fn main()
{
    // 함수가 호출코드보다 아래에 선언되도 잘 동작한다
	let ret = add(1,2);
	
	println!("{}", ret);
}

fn add(x : i32, y : i32) -> i32
{
	x + y
}
```

* 의외로 지원하지 않는 것
    * 함수 오버로딩
    * 디폴트 파라미터
    * 가변안자
    * 이게 왜 안되지 ㄷㄷ;;

```rust
fn main()
{
	println!("{}", add(1,2));

    // 아래와 같이 inner 함수 지원
        // 그냥 closuer(lambda)를쓰자;;
	fn add(x : i32, y : i32) -> i32 
	{ 
		return x + y + value;
	}
}
```

---

## 함수 포인터

```rust
fn add(x : i32, y : i32) -> i32 
{
	x + y 
}

fn main()
{
	let padd : fn(i32, i32)->i32 = add;
//	let padd : fn(i32, i32)->i32 = &add;    // 요건안됨
	
	println!("{}", add(1,2));   // 3
	println!("{}", padd(1,2));  // 3
}
```

```rust
fn add(x : i32, y : i32) -> i32 { x + y }
fn sub(x : i32, y : i32) -> i32 { x - y }

// 아래와 같이 함수 포인터를 받도록 만들어 보자
fn calc(x:i32, y:i32, pf : fn(i32, i32)->i32 ) -> i32
{
	pf(x, y)
}

fn main()
{
	println!("{}", calc(5, 3, add)); // 8
	println!("{}", calc(5, 3, sub)); // 2

	println!("{}", get_op(1)(5,3)); // 8 
	println!("{}", get_op(2)(5,3)); // 2
}

fn get_op(id : i32) -> fn(i32, i32)->i32
{
	match id
	{
		1 => add,
		2 => sub,
		_ => todo!()
	}
}
```

```rust
fn add(x:i32, y:i32) -> i32 { return x + y;}
fn sub(x:i32, y:i32) -> i32 { return x - y;}

fn main()
{
	let mut f1 : fn(i32, i32)->i32 = add;
	let mut f2 = add;       // 타입을 생략하고 타입을 추론하게 만듦
    // 이러면 문제가 f2는 함수포인터 타입이 아니라 함수타입이 된다.

	f1 = sub;
	f2 = sub;	// Error - f2는 함수 타입이기에 포인터인 함수 포인터를 담을수 없다

    // (주의!) 함수타입은 모든 함수마다 다 다름
}
```

```rust
fn add(x:i32, y:i32) -> i32 { return x + y;}
fn sub(x:i32, y:i32) -> i32 { return x - y;}

fn main()
{	
	let mut f1 : fn(i32, i32)->i32 = add;
	let mut f2 = add;

    // 함수 타입을 다시 함수포인터 타입으로 캐스팅
	let mut f3 = add as fn(i32, i32)->i32;

	// 아래 코드에서 에러를 찾아 보세요
	f1 = sub;
	f2 = sub; // error
	f3 = sub;
}
```