---
layout: default
title: "23. example) user input 받기"
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

```rust
fn main()
{
    // 사용자의 입력을 받을 준비
	let stdin : std::io::Stdin = std::io::stdin();	

    // 입력을 저장할 변수
	let mut s = String::new();

    // 읽어와라
	stdin.read_line(&mut s).unwrap();
        // unwrap() - 실패시 panic출력

	println!("{} AA", s);
	println!("{:?}", s);

    // white space를 제거하고 i32로 파싱해야한다
        // 아니면 파싱이 안됨
	let s2 = s.trim();

//	let n = s2.parse::<i32>().unwrap();
	let n = match s2.parse::<i32>()
	{
		Ok(n) => n,
		Err(e) => -1,
	};

	println!("{}", n);
}
```

* 좀 더 쉽게?

```rust
fn main() 
{
	let mut s = String::new();

	std::io::stdin().read_line(&mut s).unwrap();

	let n = s.trim().parse::<i32>().unwrap();
	
	println!("{}", n);
}
```