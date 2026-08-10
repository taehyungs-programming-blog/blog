---
layout: default
title: "Visual Studio Cross Platform 지원 방법"
parent: "Etc 🛠"
grand_parent: "Legacy Archive"
nav_order: 10
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* Visual Studio Windows지원은 당연하기에 설명은 하지 않고,
* Linux를 어떻게 Cross Platform지원할지 설명한다.

---

## Visual Studio 준비사항

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/etc/vscp-1-1.png"/>
</p>

* C++ Linux설치
* C++에 CMake관련 설치도 해줘야 한다

---

## 리눅스에 필요한 패키지

```bash
$ sudo apt-get update
 
# gdb
$ sudo apt-get install build-essentail gdb
 
# 버전확인
$ gcc --version
$ g++ --version
  # 만약 설치가 안되어 있다면
  $ sudo apt-get install g++
$ gdb --version

# openssh
$ sudo apt-get install openssh-server
$ sudo service ssh start
$ sudo systemctl enable ssh
 
# cmake
$ sudo apt-get install cmake
 
# ninja
$ sudo apt-get install ninja
# 설치가 안된다면
$ sudo apt-get install ninja-build

# << 여기부턴 옵션 >>
# git
$ sudo apt-get install git

# samba(파일공유용)
$ sudo apt-get -y install samba
$ sudo smbpasswd -a [사용자명]
$ sudo pdbedit -L # 사용자 확인
$ mkdir /share
$ sudo chmod 777 /share
$ code /etc/samba/smb.conf
"""
[share] # [] 대괄호는 섹션을 정의, 윈도우에서 접근할 때 폴더 이름이 세션안의 문자열로 보인다. 보이고 싶은 이름을 적으면 된다.
comment = samba shared directory # 간단한 공유 폴더 설명
path = /smbdir # 공유 디렉토리 경로
read only = no # 읽기 전용
writable = yes # 쓰기 전용 
guest ok = no # 다른 사용자들도 이용할 수 있도록 설정
valid user = user1 # 공유 디렉토리를 이용할 수 있는 사용자를 설정
create mask = 0777 # 파일 허가권
directory mask = 0777 # 디렉토리 허가권
"""
$ service smbd restart
```

---

## Visual Studio에서 원격 연결하는 법

* 도구 -> 옵션 -> 플랫폼 간 -> 연결 관리자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/etc/vscp-1-2.png"/>
</p>