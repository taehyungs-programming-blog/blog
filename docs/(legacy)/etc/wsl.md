---
layout: default
title: "WSL(Windows Subsystem for Linux)"
parent: "Etc 🛠"
grand_parent: "Legacy Archive"
nav_order: 10
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## WSL(Windows Subsystem for Linux)

* 윈도우에서 Linux 환경쓰기
* 환경구성 방법
    * Windows 기능 켜기/끄기
* **(주의)** Hyper-V로 켜줘야함!! [참고사이트 🌍](https://imperfectblog.tistory.com/151)
    * Linux용 Window 하위 시스템 On -> **WSL1이 설치됨**
    * 가상 머신 플랫폼 On
    * Linux 커널 업데이트 패키지 다운 후 설치-> [링크](https://wslstorestorage.blob.core.windows.net/wslblob/wsl_update_x64.msi) -> **WSL2가 설치됨**
    * Linux 배포판 설치 -> MS Store -> Ubuntu 20.04 LTS 설치 -> 실행
    * WSL2를 디폴트로 세팅 -> Power Shell `$ wsl --set-default-version 2` -> `$ wsl -l -v`
    * (추가) Visual Studio Code -> WSL 학장팩 설치

```bash
# posershell의 명령 참조

# 기본 버전 세팅
$ wsl --set-default-version 2

# 현재 버전 확인
$ wsl -l -v

# 버전올리기
$ wsl --set-version {name} 2
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/etc/os/os-1-1.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/etc/os/os-1-2.png"/>
</p>

---

## Ubuntu GNU Tool Chain 설치

* 시작 -> Ubuntu

```bash
$ sudo apt-get update
$ sudo apt-get install build-essential gdb

# 버전확인
$ gcc --version
$ g++ --version
  # 만약 설치가 안되어 있다면
  $ sudo apt-get install g++
$ gdb --version
```

```bash
# ssh server 설치
$ sudo apt-get install openssh-server
$ sudo service ssh start

# 시스템 시작시 ssh 자동실행
$ sudo systemctl enable ssh

# ssh port 및 접속방법 재설정 할 경우 수정
$ sudo nano /etc/ssh/sshd_config

# ssh 재시작
$ sudo service ssh restart

# ssh 상태확인
$ sudo service ssh status

# ssh 시작/중지
$ sudo service ssh start/stop
```

---

## VSCode와 WSL 연결

* 왼쪽 아래 **원격 창 열기** 클릭
* WSL에 접속하여 열리게 됨

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/etc/os/os-1-3.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/etc/os/os-1-4.png"/>
</p>

* 빌드해보기
* `hello.c` 생성

```cpp
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    pid_t pid;

    // fork는 리눅스용 함수이다.
    pid = fork();
    printf("Hello, WSL!");

    return 0;
}
```

```bash
$ gcc hello.c
$ ./a.out
```