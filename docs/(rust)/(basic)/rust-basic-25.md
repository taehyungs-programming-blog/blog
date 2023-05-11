---
layout: default
title: "25. reference"
parent: "(Rust 기초)"
grand_parent: "(Rust 🦀)"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```rust
//fn foo( v2 : Vec<i32> )   // 1 이건 move가 발생하게 된다
//fn foo( v2 : &Vec<i32> )  // 2 reference로 받겠다 표현
fn foo( v2 : &mut Vec<i32> )// 3 쓰기까지 지원
{
	println!("foo : {:?}", v2);
}

fn main()
{
	let mut v1 = vec![1,2,3];

//	foo(v1);
//	foo(v1.clone());        // 1 이렇게 써도 되는데 오버헤드가 있다
//	foo(&v1);               // 2 보내기도 reference로 보낸다
	foo(&mut v1);           // 3 쓰기까지 지원

	println!("{:?}", v1); 
}
```

* 호출하는 코드만 봐도 의도를 알수 있다.
    * `foo(&mut v1);` - v1을 내부에서 쓸 것이며(`mut`), move는 일어나지 않았음 한다(`&`)

```rust
fn main()
{
	let v1 = vec![0,1,2,3,4];

    // rust reference는 단순히 메모리 주소만 갖는 케이스가 있고, 추가 정보를 갖는 케이스도 있다
	let r1 = &v1;              // 메모리 주소만 갖는 경우
	let r2 = &v1[1..4];         // 메모리 주소 + 카운트까지 갖는경우

	println!("{:?}", r1);
	println!("{:?}", r2);

    // 경우에 따라 사이즈도 달라진다
	println!("{}", std::mem::size_of_val(&r1));//8
	println!("{}", std::mem::size_of_val(&r2));//16

    // r1이 파괴되면 메모리전체가 날아감
    // r2는 파괴더더라도 메모리전체가 날아가진 않는다
}
```

* 만약 v1이 r1, r2 보다 먼저 삭제된다면?
* rust는 r1, r2도 사용하지 못하게 막아준다 (rust의 장점)

```rust
fn main()
{
	let n = 10;

	// 레퍼런스를 만드는 방법
	let r1 : &i32 = &n;
	let r2        = &n;	


	// 레퍼런스를 사용해서 대상체 접근
	let v1 : i32 = *r1; // ok
//	let v2 : i32 = r1;  // error - type error

	let v3 = *r1; // v3는 i32
	let v4 = r1;  // v4는 &i32

	// println 으로 출력할때
	println!("{:p}", &n);  // n의 주소
	println!("{}",   *r1); // 10
	println!("{}",   r1);  // 10
	println!("{:p}", r1);  // n의 주소
//	println!("{:p}", *r1); // error 
	println!("{:p}", &r1); // r1자체의 주소
	println!("{:p}", &*r1);// n의 주소	
}
```

```rust
fn main()
{
	let mut n = 10;

//	let r1 : &i32 = &n; 
//	*r1 = 20;

	let r2 : &mut i32 = &mut n;     // 읽고 쓰기가 모두 가능하게 해줘!
	*r2 = 20;


	println!("{}", n);
}
```

```rust
fn main()
{
	let v1 = 10;
	let v2 = 20;

//	let r1 = &v1;
    // reference 자체의 mutable을 의미한다
	let mut r1 = &v1;

	r1 = &v2;

	println!("{}", *r1);


	let mut n = 10;
	let mut r : &mut i32 = &mut n;	
}
```

```rust
fn swap( r1 : &mut i32 , r2 : &mut i32 )
{
	let tmp = *r1;
	*r1 = *r2;
	*r2 = tmp;
}

fn main()
{
	let mut x = 10;
	let mut y = 20;

	swap(&mut x, &mut y );

	println!("{}", x); 	// 20
	println!("{}", y);	// 10

}
```