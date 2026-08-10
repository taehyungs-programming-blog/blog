---
layout: default
title: "29. closure"
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

## closure

```rust
fn twice(e : i32) -> i32
{
    e * 2
}

fn main()
{
    let arr1 = [1,2,3,4];

    // 이런 closure(lambda)로 표현할 방법이 없을까?
    let arr2 = arr1.map(twice);

    println!("{:?}", arr2);
}
```

```rust
fn main()
{
    let arr1 = [1,2,3,4];

    let arr2 = arr1.map(|e| e * 2);

    println!("{:?}", arr2);
}
```

```rust
fn main()
{
//	let f1 = |x:i32, y:i32| -> i32 { x+y };

//	let f1 = |x, y| { x+y };
	let f1 = |x, y| x+y;

	let ret1 = f1(1, 2); // i32
//	let ret2 = f1(1.1, 2.2); // f64 error
	

	println!("{}", ret1); // 3
}
```

```rust
fn twice(e : i32) -> i32
{
	e * 2  // e + value
}

fn main()
{
	let value = 3;	// 사용자 입력값

	let arr1 = [1,1,1,1];

    // 외부 데이터 캡쳐도 가능할까? -> 가능!(단, 지역변수에 접근가능)
	let arr2 = arr1.map(|e| e + value );

	println!("{:?}", arr2);	
}
```

---

## closure와 capture

```rust
fn main()
{
	let mut v1 = 0;

    // f자체가 closure가 된다
        // || -> 입력은 없음
        // 출력은 println!("{}", v1);
	let f = || println!("{}", v1);
	
	f();

//	v1 = 20;    // error - closure에서 v1을 대여중
	println!("{}", v1); // immutable한 대여로 읽을순 있다
	
	f();

	v1 = 20;
}
```

```rust
fn main()
{
	let mut v1 = 0;

// closure 내부에서 변수 수정이 될까?
//	let     f = || v1 = 20; // 내부에서 수정할 것임을 알려달라
	let mut f = || v1 = 20; // muterble하게 대여함

	println!("{}", v1);     // error - mutable하게 대여해 읽을수도 없다
	
	f();

	println!("{}", v1);
}
```

```rust
fn main()
{
	let mut n = 10;
	let     s = "ABCD".to_string();

    // imutable하게 대여하기에 읽을순 있으나 쓸순 없게 됨.
    //	let f = || println!("{},{}", n, s);

    // move를 적으면 closure내부적으로 사용할 n, s를 별도로 생성함.(원본이 아님)
	let f = move || println!("{},{}", n, s);

	println!("{}", n);
//	println!("{}", s); // error - string은 reference를 closure s에게 뺏기게 됨
	n = 20;

	f();
}
```

```rust
fn main()
{
	let mut n = 10;

    // 내부에서 값의 수정을 알리기 위해 mut f를 선언
	let mut f = move || n = 20;

	f();

	println!("{}", n);
}
```

---

## closure argument

```rust
fn foo(f : fn(i32, i32)->i32)
{
    println!("{}", f(1, 2));
}

fn main()
{
    let v = 10;

    // closure를 함수 포인터로 보낼수 있음.
    foo(|x:i32, y:i32| x + y);

    // Error-
        // 캡쳐하지 않은 Closure만 함수 포인터 변환이 가능
        // 가능하게 하고싶다면 -> Generic 활용
    foo(|x:i32, y:i32| x + y + v);
}
```

```rust
fn foo<F>(f : F)
    where F : Fn(i32, i32) -> i32   // immutable하다 알린다
{
    println!("{}", f(1, 2));
}

fn main()
{
    let mut v = 10;

    foo(|x:i32, y:i32| x + y + v);

    // 만약 closure내에서 값을 바꾼다면?? (아래 참조)
    foo(|x:i32, y:i32| v = 20; x + y + v);
}
```

```rust
fn foo<F>(mut f : F)
    where F : FnMut(i32, i32) -> i32
{
    println!("{}", f(1, 2));
}
```