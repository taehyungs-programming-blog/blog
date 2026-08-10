---
layout: default
title: "06. macro"
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

## dbg!() 매크로

```rust
fn main() 
{
	let v1 = 10;

	println!("{v1}"); // 10
	dbg!(v1);         // [*.rs:7] v1 = 10   - 이런식으로 출력됨

//	let a = v1 + 20;
	let a = dbg!(v1 + 20);

//	if a == 30
	if dbg!(a == 30)
	{
		println!("a is 30");
	}
}
```

---

## cfg!() 매크로

```rust
fn main()
{
	// 1 cfg 매크로
        // 현재 운영체제 검사
//	if cfg!(target_os="linux")
	if cfg!(linux)
//	if cfg!(not(linux))
	{
		println!("linux");
	}
	else
	{
		println!("not linux");
	}	

	do_work();
}


// 2 #[cfg( ... )] attribute
#[cfg(target_os="linux")]
fn do_work() { println!("linux"); }

#[cfg(not(target_os="linux"))]
fn do_work() { println!("not linux"); }


// 3 OS 이름
// => windows, macos, ios, linux, android, freebsd, dragonfly, openbsd, netbsd
// => "The Rust Reference" 문서 참고
```

---

## error return

```rust
fn main()
{
//	ex1();
//	ex2();
	println!("main continue");
}

// 1 process 를 종료 하는 "함수"
fn ex1() { std::process::exit(0); }


// 2 process 를 종료 하고 메세지와 디버깅 정보를 출력.
// => 복구 할수 없는 오류 발생시 사용
fn ex2() { panic!("복구할수 없는 에러 발생"); }


// 3 실행시간이 아닌 "컴파일 시간"에 에러 발생
#[cfg(not(target_os="linux"))]
compile_error!("not linux");
```

---

## debug

```rust
// assert!(), debug_assert!()
fn main()
{
	let v1 = 10;
	let v2 = 10;

	// 1 assert : "실행 시간에" 값의 유효성 조사
	// => () 안의 표현식이 거짓일 경우 panic!() 발생
	assert!(v1 == v2);
	assert_eq!(v1 * 2, 20);
	assert_ne!(v1 * 2, 20);


	// 2 debug_assert : debug 버전에서만 동작
	//				     debug_assertions 가 정의된 경우만 동작.
	debug_assert!(v1 == v2);
	debug_assert_eq!(v1 * 2, 20);
	debug_assert_ne!(v1 * 2, 20); // panic!(), 단, 최적화 하지 않을 때만
}
```

* rustc 컴파일러 최적화 옵션
    * `-C opt-level=0` : debug_assertions 이 정의 되어있음. 디폴트 값
    * `-C opt-level=1 또는 2` : debug_assertions 이 정의 안됨.

* 컴파일 옵션에 따른 debug_assert!() 동작 여부
    * `rustc macro4.rs -C opt-level=1`
    * `macro4.exe` => debug_assert!() 동작안함
    * `rustc macro4.rs -C opt-level=1 -C debug_assertions`
    * `macro4.exe` => debug_assert!() 동작함.