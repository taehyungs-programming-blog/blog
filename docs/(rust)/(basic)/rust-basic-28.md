---
layout: default
title: "28. slice"
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
fn main()
{
	let v = vec![0,1,2,3,4];

	let r1 : &Vec<i32> = &v;
	let r2 : &[i32] = &v[1..4];
//	let r3 : &[i32] = &v[0..v.len()];   // 아래와 동일한 표현이다
	let r3 : &[i32] = &v[..];

    // Slice - 연속된 메모리의 일부를 사용하는 것

	println!("{:?}", v);  // [0,1,2,3,4]
	println!("{:?}", r1); // [0,1,2,3,4]
	println!("{:?}", r2); // [1,2,3]
	println!("{:?}", r3); // [0,1,2,3,4]

	println!("{:p}, {:p}", &v, v.as_ptr());
	println!("{:p}", &v[0]); // 버퍼 주소
	println!("{:p}, {:p}", &r1, r1); // v주소
	println!("{:p}, {:p}", &r2, r2); // 
	println!("{:p}, {:p}", &r3, r3); // 버퍼주소


    // 타입을 생략하고 쓸때가 많은데, 출력해 보는것도 좋은 방법이다
	let r4 = &v[1..3];

	print_type_of(&r4);
	
}

fn print_type_of<T>(_: &T) 
{
    println!("{}", std::any::type_name::<T>())
}
```

```rust
fn main()
{
	let mut v = vec![1,3,5,7,9,2,4,6,8,10];
	
	println!("{:?}", v);

//	v.sort();

    // 일정 부분만 정렬이 될까? -> 됨
	let s = &mut v[2..8];
	s.sort();

	println!("{:?}", v);

//	for e in v
	for e in &v[2..8]
	{
		print!("{}, ", e);
	}
}
```

```rust
fn main()
{
	let v = vec![0,1,2,3,4];

	let r2;
	{
		let r1 = &v[0..4];
		r2 = &r1[1..3];
	}

	println!("{:p}", v.as_ptr());
//	println!("{:p} {:?}", r1, r1);
	println!("{:p} {:?}", r2, r2);
}
```

---

* 문자열의 slice `&str`

```rust
fn main()
{
	let s = String::from("ABCDEFG");
	
	let r1 = &s;	
	let r2 = &s[2..5];	
	
	println!("{}", s);  // ABCDEFG
	println!("{}", r1); // ABCDEFG
	println!("{}", r2); // CDE


	let r3 = &s;
	let r4 : &str = &s; // &s[..] 이렇게 캐스팅이 된다

	print_type_of(&r3); // &String
	print_type_of(&r4); // &str

	println!("{:p}, {:p}", &s, s.as_ptr());
	println!("{:p}, {:p}", r3, r4);	
}

fn print_type_of<T>(_: &T) 
{
    println!("{}", std::any::type_name::<T>())
}
```

```rust
fn main()
{
	let s1 = String::from("ABCD");
	let s2 = String::from("ABCD");

	println!("{:p}", s1.as_ptr());
	println!("{:p}", s2.as_ptr());

	let s3 = "ABCD";
	let s4 = "ABCD";

    // 타입은 &str
	print_type_of(&s3);
	print_type_of(&s4);

    // mutable이 아니기에 상수메모리에 올라가며 같은 주소가 리턴된다
	println!("{:p}", s3);
	println!("{:p}", s4);	
}

fn print_type_of<T>(_: &T) 
{
    println!("{}", std::any::type_name::<T>())
}
```

```rust
fn f1( s : String )  {} // s = "ABCD"
fn f2( s : &String ) {}
fn f3( s : &str )    {}

fn f4( s : &mut String ) {}

fn main()
{
	let s = String::from("ABCD");

//	f1(s);		// ok. 문자열 move
//	f1("ABCD"); // error

//	f2(&s);		// ok
//	f2("ABCD");	// error

	f3(&s);		// ok - let r :&str = &s; - string slice되며 가능
	f3("ABCD");	// ok
}
```