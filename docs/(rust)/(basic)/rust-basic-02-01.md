---
layout: default
title: "02-01. 들어가기 전, 무작정 따라하기"
parent: "(Rust 기초)"
grand_parent: "(Rust 🦀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Reference ✨](https://rinthel.github.io/rust-lang-book-ko/ch02-00-guessing-game-tutorial.html)

## 프로젝트 세팅

```bash
$ cargo new guessing_game --bin
```

* `--bin` - 실행 가능한 프로그램을 생성 
* `--lib` - 라이브러리용 프로젝트를 생성

```rust
// main.rs

fn main() {
    println!("Hello, world!");
}
```

* Tip) VSCode 빌드 - Ctrl + Alt + n

---

## 코드 작성

### 입력받아보기

```rust
use std::io;

fn main() {
    println!("Guess the number!");

    println!("Please input your guess.");

    // string 생성
        // rust는 기본적 immmutable이라 mut을 선언
    let mut guess = String::new();

    // io read
        // &mut - guess의 주소를 mutable
    io::stdin().read_line(&mut guess)
        .expect("Failed to read line");

    println!("You guessed: {}", guess);
}
```

### 외부 라이브러리 활용

```ini
# Cargo.toml
[dependencies]
rand = "0.8.4"
```

```bash
$ cargo clean
$ cargo build
```

```rust
// rust 2018부터 extern crate 가 필요없음.

use std::io;
use rand::Rng; // rand와 Rng 트레잇을 사용하기 위한 선언

fn main() {
    println!("Guess the number!");

    // rand::thread_rng()를 사용하여 로컬 스레드의 RNG를 가져오고, 1부터 100까지의 범위에서 난수를 생성
    let secret_number = rand::thread_rng().gen_range(1..101);

    println!("The secret number is: {}", secret_number);

    println!("Please input your guess.");

    let mut guess = String::new();

    // 표준 입력을 통해 사용자로부터 추측값을 입력받음
    io::stdin().read_line(&mut guess)
        .expect("Failed to read line");

    println!("You guessed: {}", guess);
}
```

```rust
extern crate rand;

use std::io;
use std::cmp::Ordering;
use rand::Rng;

fn main() {
    println!("Guess the number!");

    let secret_number = rand::thread_rng().gen_range(1, 101);

    println!("The secret number is: {}", secret_number);

    println!("Please input your guess.");

    let mut guess = String::new();

    io::stdin().read_line(&mut guess)
        .expect("Failed to read line");

    let guess: u32 = guess.trim().parse()
        .expect("Please type a number!");

    println!("You guessed: {}", guess);

    match guess.cmp(&secret_number) {
        Ordering::Less    => println!("Too small!"),
        Ordering::Greater => println!("Too big!"),
        Ordering::Equal   => println!("You win!"),
    }
}
```

