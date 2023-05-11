---
layout: default
title: "16. if & match"
parent: "(Rust 기초)"
grand_parent: "(Rust 🦀)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```rust
fn main()
{
	let score = 75;

    // 이런 표현에 주목하자 
        // if문으로 변수를 초기화 할 수 있음.
	let result1 = if score > 70 
				  {"pass"} else {"fail"};
        // 여기서 드는 의문점 왜 if문 끝에 ;이 붙어있나?
        // 표현식으로 result1에 결과를 넣어주기 위함이다.

    // 보통의 if문의 경우 아래와 같이 ;가 필요없다
    if score > 75
    {
    } 	// <== ; 필요 없음	


    // 역시 case문도 마찬가지
	let result2 = match score 
				  {
						70 ..= 100 => "pass",
						_          => "fail",
				  };

	println!("{}, {}", result1, result2);
}
```

* 좀 더 상세히 살펴보자

* if의 특징

```rust
fn main()
{
	let score = 70;

	// 조건식은 () 를 사용 안함, 사용하면 경고
	if score > 90
	{
		// 실행할 문장이 없더라도 {}가 팔요함
	}
	else if score > 60
	{
	}
	else 
	{
	}
}
```

* match의 특징

```rust
fn main()
{
	// match 는 모든 값을 cover 해야 한다.
	let n = 3;
	match n 
	{
		0 => println!("zero"),
		1 => println!("one"),
//		x => println!("other"), // 아래가 더 깔끔한 방법
		_ => println!("other"), 
	}

	let b = true;
	match b
	{
		true  => println!("true"),
		false => println!("false"),
	}

	// expression 으로 사용되면 
	// 동일 타입을 반환해야 한다.

	let value = match n 
				{
					0 => 10,
					1 => 20,
				//	_ => 3.4,
					_ => 0,
				};

}
```

```rust
fn main()
{
	let v1 = 10;
	let v2 = 20;
	let t = (10, 20);

	// tuple matching
//	match (v1, v2)
	match t
	{
		(10, 10) => println!("10, 10"),
		(10, 20) => println!("10, 20"),
		_        => println!("......"),
	}

	match t
	{
		(a, 10) => println!("10, 10"),
		(a, 20) => println!("{}, 20", a),
		_       => println!("......"),
	}	

	// match guard
	let t2 = (15, 20);
	match t2 
	{
		(a, b) if a > 10 => println!("1st arm"),
		_       => println!("......"),
	}

	// @ 표기법
        // n3가 3일때 num @ 3으로 들어가고 num = n3이 된다
	let n3 = 3;
	match n3
	{
		num @ 3 => println!("{}", num),
		_       => println!("......"),
	}	


	// array match
	let arr = [1,2,3];

	match arr 
	{
		[a, b, 3] => println!("1"),
		[a, b, c] => println!("2"),
	}	
}
```
