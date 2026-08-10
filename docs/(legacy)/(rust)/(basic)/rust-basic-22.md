---
layout: default
title: "22. result"
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

* Option과 비슷해 보이는데 Result는 차이가 뭔가?
    * Option - `None` 없다 or `Some(T)` 있다 + 값
    * Result - `Ok(T)` 성공 or `Err(E)` 실패 + 이유

```rust
fn main()
{
	let ret = std::fs::remove_file(
							"Non-existent file");	

	println!("{:?}", ret);

	print_type_of(&ret);	// core::result::Result<(), std::io::error::Error>
}


fn print_type_of<T>(_:&T)
{
	println!("{}", std::any::type_name::<T>());
}
```

```
error[E0425]: cannot find function `print_type_of` in this scope
 --> .\hellorust.rs:8:2
  |
8 |     print_type_of(&ret);
  |     ^^^^^^^^^^^^^ not found in this scope

error: aborting due to previous error

For more information about this error, try `rustc --explain E0425`.
```

* 에러의 처리

```rust
fn main()
{
	let ret = std::fs::remove_file(
						"Non-existent file");

	if let Err(e) = ret 
	{
		println!("fail : {:?}", e);
	}				
	
	/*
	match ret
	{
		Ok(v)  => println!("success"),
		Err(e) => println!("fail : {:?}", e),
	}
	*/
}
```

```rust
// 어떻게 활용하는지 보자

fn foo(x : i32) -> Result<i32, i32>
{
	if x < 0 { Err(x) }
	else { Ok(x) }
}

fn main()
{
	let ret = foo(30);

	match ret 
	{
		Ok(v)  => println!("Ok  : {}", v),
		Err(v) => println!("Err : {}", v),
	}

	let value = match foo(30)
	{
		Ok(v)  => v,
		Err(v) => panic!("fail"),
	};


    // 위를 Result안의 unwrap이 해준다
	let value = foo(-30).unwrap();
}
```

```rust
// Result가 제공하는 함수

fn foo(x : i32) -> Result<i32, i32>
{
	if x < 0 { Err(x) }
	else { Ok(x) }
}

fn main()
{
	let ret1 = foo(30).is_ok();	
	let ret2 = foo(30).is_err();
	let ret3 = foo(30).ok();  
	let ret4 = foo(30).err(); 

    // 실패시 panic
	let value1 = foo(10).unwrap();
	let value2 = foo(-20).expect("panic message");

	println!("{}, {}", value1, value2);

}
```

---

```rust
use std::io::prelude::*;

// 아래 코드를 보면 파일을 만들고 쓰고 쓰고 하는데 실패시 Error 리턴한다
    // 좀 더 간단히 쓸순 없을까?
fn write_file() -> std::io::Result<()> 
{
    let mut file = match std::fs::File::create("sample.txt") 
					{
						Err(e) => return Err(e),
						Ok(f)  => f,
					};

    if let Err(e) = file.write_all(b"hello ") 
	{
        return Err(e)
    }
    if let Err(e) = file.write_all(b"world") 
	{
        return Err(e)
    }

    Ok(())
}

fn main()
{
	let ret = write_file();
}
```

```rust
use std::io::prelude::*;

fn write_file() -> std::io::Result<()> 
{
    // ? 연산자로 실패시 에러리턴
    let mut file = std::fs::File::create("sample.txt")?;
				
    file.write_all(b"hello ")?;
    file.write_all(b"world")?;
	
    Ok(())
}

fn main()
{
	let ret = write_file();
}
```