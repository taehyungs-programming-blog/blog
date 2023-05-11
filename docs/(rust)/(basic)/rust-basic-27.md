---
layout: default
title: "27. life-time"
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
	let long = 10;
	
	let r;
	{
		let short = 10;

//		r = &long; 	
		r = &short;	
	}

	println!("{}", r);  // error - short는 이미 수명이 다함.
}
```

```rust
fn one( x : &i32 ) -> &i32
{
	x
}

fn main()
{
	let long = 10;
	
	let r;
	{
		let short = 10;

    //
//		r = one(&long); 	// ok
		r = one(&short);	// error
        // 에러가 나야할게 결국은 r로 가리키니 수명이 다한 변수를 가리키면 안됨
	}

	println!("{}", r);
}
```

```rust
// <'a> 생명주기를 표현해 준다 - 'a가 붙은 애들은 생명주기를 같이한다는 표현
fn two<'a>( x : &'a i32, y : &i32 ) -> &'a i32
{
	x	
//	y
}

fn main()
{
	let n = 0;
	let long = 10;
	
	let r;
	{
		let short = 10;

//		r = two(&long, &n); 	// ok	
		r = two(&short, &n);	// error
	}

	println!("{}", r);
}
```

```rust
fn max<'a>( x : &'a i32, y : &'a i32 ) -> &'a i32
{
    // 상황에 따라 다른걸 리턴한다면?
	if x > y { x }	
	else     { y }
}

fn main()
{
	let long = 10;
	let short = 10;
	let r;
	{
	//	let short = 10;

        // 둘 중 짧은 수명에 채택이 된다.
		r = max(&long, &short); 	
	}

	println!("{}", r);
}
```

---

* static 수명 - 프로그램 수행중 파괴되지 않는 수명

```rust
fn f1(x : &'static i32 )
{
}

fn main()
{
	let n = 10;
	static N : i32 = 10;

//	f1(&n); // error - 절대 파괴되지 않을 애만 넣어주세요!
	f1(&N);
}
```

```rust
// 보통은 반환에 많이 쓰인다
fn f1() -> &'static i32
{
//	let n = 10;
//	&n      // 지역변수를 reference로 반환해 버린다?? 함수 호출뒤에 사라지는데?
	static N :i32 = 10;
	&N

}

fn main()
{
	let r = f1();
}

fn f2() -> &'static str
{
	"hello"
}
```

```rust
struct MyType<'a>
{
	data  : i32,

    // 이렇게 까지 수명을 표기해야한다
	refer : &'a i32
}

fn main()
{
	let n = 10;	
	let mt;
	{
	//	let n = 10;	

		mt = MyType{data:10, refer:&n};
	}
	println!("{}", mt.refer);
}
```