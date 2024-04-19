---
layout: default
title: "03-04. 슬라이스"
parent: "(Rust 기초)"
grand_parent: "(Rust 🦀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Reference ✨](https://doc.rust-kr.org/ch04-03-slices.html)

* 슬라이스 (slice) 
    * 컬렉션 (collection) 을 통째로 참조하는 것이 아닌, **컬렉션의 연속된 일련의 요소를 참조**하도록 해줍니다. 슬라이스는 참조자의 일종으로서 **소유권을 갖지 않습니다.**

* 그런데 이게 왜 필요하지?
    * 이런경우를 고려해 보자.
    * 아래 `first_word`와 같이 특정 문자의 공백을 찾아주는 함수가 있다.

```rust
fn first_word(s: &String) -> usize {
    let bytes = s.as_bytes();

    for (i, &item) in bytes.iter().enumerate() {
        if item == b' ' {
            return i;
        }
    }

    s.len()
}
```

* 그런데 실수로 아래와 같이 코딩을 했다면?

```rust
fn main() {
    let mut s = String::from("hello world");

    let word = first_word(&s); // word는 값 5를 받습니다

    s.clear(); // 이 코드는 String을 비워서 ""으로 만듭니다

    // 여기서 word에는 여전히 5가 들어있지만, 이 5를 의미있게 쓸 수 있는
    // 문자열은 더 이상 없습니다.
}
```

* 그래서 해결책은 뭔데?
    * 참조와 index를 한번에 받자!

```rust
let s = String::from("hello world");

let hello = &s[0..5];
let world = &s[6..11];
```

* 자! 그럼 개선된 방향으로 `first_word`를 수정해 보자

```rust
fn first_word(s: &String) -> &str {
    let bytes = s.as_bytes();

    for (i, &item) in bytes.iter().enumerate() {
        if item == b' ' {
            return &s[0..i];
        }
    }

    &s[..]
}
```

```rust
fn main() {
    let mut s = String::from("hello world");

    let word = first_word(&s);

    s.clear(); // 에러! - Wow!

    println!("the first word is: {}", word);
}
```

---

## 정리

* 소유권, 대여, 슬라이스는 러스트가 컴파일 타임에 메모리 안정성을 보장하는 비결입니다. 
* 여타 시스템 프로그래밍 언어처럼 프로그래머에게 메모리 사용 제어 권한을 주면서, 어떠한 데이터의 소유자가 스코프를 벗어날 경우 자동으로 해당 데이터를 정리하는 것이 가능하죠. 
* 또한 제어 코드를 추가 작성하고 디버깅할 필요가 사라지니 프로그래머의 일이 줄어드는 결과도 가져옵니다.
