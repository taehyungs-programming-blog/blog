---
layout: default
title: "03-00. rust문법 훑어보기"
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

* [Reference ✨](https://rinthel.github.io/rust-lang-book-ko/ch03-01-variables-and-mutability.html)

* 이후 더 상세히 살펴보니 여기선 간단히 보자.

## 변수

* 기본적으로 rust의 변수는 immutable이다.

```rust
fn main() {
    let x = 5;
    println!("The value of x is: {}", x);
    x = 6;  // Error!
    println!("The value of x is: {}", x);
}
```

```rust
// 단순 읽기는 가능!
fn main() {
    let x = 5;

    let x = x + 1;

    let x = x * 2;

    println!("The value of x is: {}", x);
}
```

```rust
// preview 1. variable, array, tuple

fn main() 
{
	// 1 변수 선언
	let v1 : i32 = 10;		// 타입(i32, 32bit signed integer, 다른 언어의 int )
	let v2       = 10;		// 타입 생략 가능. 초기값 10으로 타입 추론

	println!("{v1}, {v2}");	// 변수값 출력

	// 2 array : "같은 타입" 의 값을 여러개 보관
		// 참고) 메모리에 일자로 리니어하게 들어감
	let arr = [1,2,3,4,5];	
	let v3  = arr[0];	

	// 3 tuple : "다른 타입" 의 값을 여러개 보관
		// 참고) tuple도 메모리공간엔 일자로 들어감(리니어하게)
		// 아마 랜덤액세스가 목적인듯?
	let tp = (1, 3.4, 'A');
	let v4 = tp.0;	
	let v5 = tp.1;

    // 4 reference 변수
    let r1 : &i32 = &v1;
    let r2 = &v2;
}
```

```rust
fn main()
{
	let n1 = 10;
	std::println!("{:p}", &n1);     // 주소값 출력

	let r1 : &i32 = &n1;
	
	std::println!("{:p}", r1);	    // 주소값 출력
	std::println!("{}",  *r1);	    // 10
}
```

---

## Closure(lambda)

```rust
// preview 2. 함수와 클로져(Closure)

// 1 함수 구현. 호출 코드 보다 아래 부분에 있어도 상관없음.
fn add(a : i32, b : i32) -> i32	
{
	return a + b;
}

fn main()
{
	// 2 함수 호출
	let ret1 = add(10, 5); 	

	
	// 3 익명의 함수 "Closure", 일부 다른 언어의 "lambda"
	let sub = |a:i32, b:i32| return a - b;

	let ret2 = sub(10, 5);

	println!("{ret1}, {ret2}");
}
```

---

## 제어문

```rust
// preview 3. 제어문 - if, match

fn main()
{	
	let score = 70;	

	// 1 if 문
	if score >= 60		// 조건식에 () 가 필요 없다.
	{					// 실행할 문장이 한줄이라도 반드시 {}이 필요 하다.
	}
	else
	{
	}

	// 2 match 문 - 아주 강력한 기능을 가진 제어문
	match score
	{
		0       => println!("zero"),	// (주의) ; 이 아닌 , 사용
		1 ..=39 => println!("fail"),	// 1 ~ 39
		40..=59 => println!("reexam"),	// 40 ~ 59
		_       => println!("pass"),	// 나머지의 경우
	}
}
```

---

## 반복문

```rust
// preview 4. 반복문 - while, loop, for

fn main()
{	
	// 1 imutable vs mutable 
	//  let i = 0;	// i 값을 변경할 수 없다. immutable
	let mut i = 0;	// i 값을 변경할 수 있다. mutable

	// 2 while 문 - 조건에 따른 반복
	while i < 10
	{
		println!("{i}");
		i += 1;		// 	"++i" 는 안됨 - ++ 연산자 제공 안함
	}	

	// 3 loop 문 - 무한 루프 ( while true 과 동일 )
	loop
	{
		// break    : 반복문 탈출
		// continue : 반복문의 처음으로 이동
		break;
	}

	// 4 for-in : iterator 를 가진 타입(대부분 collection)의 모든 요소를 차례대로 열거
	let arr = [1,2,3,4,5];
	
	for e in arr
	{
		println!("{e}");
	}

	// 0~9 까지 반복
	for e in 0..10
	{
		println!("{e}");
	}
}
```

---

## struct

```rust
// preview 5. 사용자 정의 타입 만들기 ( 객체지향 프로그래밍 특징 )

// 1 struct 키워드 사용. class 키워드가 없고, 상속 문법을 지원 하지 않음.
struct Point 
{
	x : i32, // Filed	
	y : i32, // (주의) ; 이 아니고 , 	
}

// 2 method 추가
    // ??? impl???? 이게 뭔가? 일단은 받아들이자.
impl Point 
{
	fn get_x(&self) -> i32 
	{
		return self.x;
	}
}

fn main() 
{	
	// 3 객체 생성
	let pt = Point{x:10, y:20};

	// 4 method 호출
	let x = pt.get_x();

	println!("{x}");
}
```

---

## enum

```rust
// preview 6. enum #1

// 1 enum으로 새로운 타입 만들기 
enum ExamResult
{
	Pass,
	Fail,
}

fn main()
{
	// 2 enum 변수 생성
	let result = ExamResult::Pass;
	
	// 3 match 를 사용해서 enum 값에 따른 코드 작성
	match result
	{
		ExamResult::Pass => println!("pass"),	
		ExamResult::Fail => println!("fail"),
	}
}
```

```rust
// preview 7. enum #2   

// 1 각 항목(variants) 에는 연관된 값을 보관 가능.
enum ExamResult
{
	Pass(i32),	// Pass 와 연관된 i32 타입의 값을 한개 보관 가능.
	Fail,
}

fn main()
{
	let result1 = ExamResult::Pass(85);
	let result2 = ExamResult::Fail;

	// ❷ match 를 사용한 patter matching 으로 연관된 값 얻기
	match result1
	{
        // score에 85가 같이 저장된다.
		ExamResult::Pass(score) => println!("pass : {score}"),
		_        				=> println!("fail"),
	}
}
```

---

## 표준 라이브러리

```rust
// preview 8. 표준 라이브러리 사용 - 다양한 함수, 타입을 "Crates" 와 "Module" 이라는 개념으로 제공 
// use std::cmp::max; 

fn main()
{
	// 1 표준라이브러리 사용
        // std:: - 이런걸 namespace라 부르지 않고 Crate라 부른다
	let ret1 = std::cmp::max(10, 5); // std 라는 이름의 "Crate" 안에 있는
									 // cmp "Module" 안에 있는
									 // max "Function" 사용 
	// 2 use 사용
	use std::cmp::max;   
	let ret2 = max(10, 5);

	println!("{ret1}, {ret2}"); 
	
	// 3 표준 라이브러리에는 함수뿐 아니라 다양한 타입도 제공
	let list1 = std::collections::LinkedList::from([1, 2, 3]);
											// ^ LinkedList 타입 안에 있는 
											//   연관 함수 (associated function) 호출
											//   (다른 언어의 static method 와 유사한 )
	use std::collections::LinkedList;
	let list2 = LinkedList::from([1, 2, 3]);
}
```

---

## move, copy

```rust
fn main()
{
	let n1 = 10;
	let s1 = String::from("ABCD");
	let s2 = String::from("ABCD");

	let n2 = n1;		// copy 
	let s3 = s1;		// move - copy아님 주의
	let s4 = s2.clone();// copy

	println!("{}", n1);
	println!("{}", s1); // error - move되었기에 
	println!("{}", s2);	// ok
}
```