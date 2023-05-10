---
layout: default
title: "01. 환경설정"
parent: "(Rust 기초)"
grand_parent: "(Rust 🦀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Rust 설치

* 간단한 코딩은 [Rust Playground](https://play.rust-lang.org/)에서 하자

* [Visual Stuio Build Tools C++ 다운/설치](https://visualstudio.microsoft.com/ko/visual-cpp-build-tools/) - 이미 Visual Studio 최신 버전이 설치되어 있다면 설치 하지 않아도 됨
* [Rust Lang 설치](https://www.rust-lang.org/tools/install)

```bash
# 설치확인
$ rustc --version
```

---

## Rust Build

* [Get Code 🌍](https://github.com/codenuri/RUST)

### 1. 명령어 사용

```rust
fn main()
{
    println!("hello, world");
}
```

* *.rs 확장자로 저장한다(파일의 저장은 UTF-8로 하자)
* `$ rustc hello.rs`로 빌드한다

### 2. Gargo 사용

* 폴더를 하나 생성 후 해당 폴더에서 `$ cargo new {projectName}`
* 여러 소스관리 파일이 생성되며 src\main.rs가 생성됨을 확인할 수 있다
* `$ cargo run`으로 빌드 & 실행

---

## Rust Update / Uninstall

```bash
$ rustup update
$ rustup self uninstall
```

---

## Visual Studio Code Extension

* rust-analyzer
* crate - Seray Uzgur : gargo.toml 편집 지원