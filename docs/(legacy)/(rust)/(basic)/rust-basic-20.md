---
layout: default
title: "20. error 처리"
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
fn db_backup()
{
	let test = DropTest;

	println!("DB BACKUP");

//	std::process::exit(-1);	// 프로세스 강제종료
// 단, Drop Method (소멸자)가 안불린다

    // panic은 Drop Method가 호출됨.
        // panic은 thread의 종료임
	panic!("fail db_backup");
}

fn db_remove()
{
	println!("DB REMOVED")
}

fn main()
{
	db_backup();
	db_remove();
}


/*
    * Drop Method *
*/

struct DropTest;

impl Drop for DropTest {
    fn drop(&mut self) {
        println!("**== drop called ==**");
    }
}
```

---

```rust
fn db_backup() -> i32
{
	-1   // 함수의 반환값으로 실패를 전달?
    // 단, 반환값과 실패가 명확하지 않음 혹은 사용이 되었나 확인이 불가능
}

fn db_remove()
{
	println!("DB REMOVED")
}
   
fn main()
{
	db_backup();

	db_remove();
}
```

```rust
// attribute로 사용을 강제할 수 있다.
#[must_use]
fn db_backup() -> i32
{
	-1
}
```