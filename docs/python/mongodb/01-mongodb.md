---
layout: default
title: "1. 환경설정"
parent: "MongoDB 💾"
grand_parent: "Python"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 사전사항

* [Anaconda Individual Edition 🌎](https://www.anaconda.com/products/individual) : Python 3.X 버전으로 설치할 것.

* **설치 확인**
    * 시작 -> Anaconda Navigator

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-1-1.png"/>
</p>

---

## MongoDB 준비

* [MongoDB Server 다운 🌍](https://www.mongodb.com/try/download/community)
* 환경변수 설정
    * `C:\Program Files\MongoDB\Server\5.0\bin` 을 **환경변수 path** 내부에 넣는다.
* `C:\data\db` 폴더 생성

```bash
# 설치확인
$ mongod
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-2-1.png"/>
</p>

* 마지막줄 Wating for connections 확인

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-2-2.png"/>
</p>

---

## Windows Machine MongoDB 서버로 사용하기

👑 mongod.cfg 수정이 필요하다면 수정 후(IP Bind 설정 등)<br>
👑 윈도우 서비스에서 MongoDB재시작<Br>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-2-13.png"/>
</p>

👑 외부에서 접속을 위해 인바운드 규칙 수정<br>
👑 설정 -> 네트워크 및 인터넷 -> Windows 방화벽 -> 고급 설정 -> 인바운드 규칙

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-2-14.png"/>
</p>

---

## (참고) AWS의 MongoDB

* AWS Free Tier(무료)는 12개월만 지원되기에 새로운 아이디를 준비하도록 하자.
* 👺 **할 일**
    * **EC2** 를 생성
    * **Elastic IP(탄력적 IP)** 생성 - 고정 IP 할당
    * EC2 서버에 접속

### **EC2** 를 생성

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-2-3.png"/>
</p>

* 다음 화면에서 설정이 필요하기에 인스턴스 세부 정보 구성을 누르자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-2-4.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-2-5.png"/>
</p>

* 설정이 끝나면 keypair를 주는데 잘 보관하도록 하자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-2-6.png"/>
</p>

### **Elastic IP(탄력적 IP)** 생성 - 고정 IP 할당

* 인스턴스 -> 네트워크 및 보안 -> 탄력적 IP

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-2-7.png"/>
</p>

* 탄력적 IP 주소 할당 -> IP를 할당받고 -> 작업 -> IP 주소 연결

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-2-8.png"/>
</p>

* 마지막으로 인스턴스에 IP가 잘 할당되었나 확인

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-2-9.png"/>
</p>

### EC2 서버에 접속

* [putty 사용예정 🌍](https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html)
* .pem(keypair)를 .ppk로 변환해야 한다.
    * putty key genertor 실행
    * .pem 로드
    * save private key로 저장(.ppk로 저장)
* putty에 .ppk 적용
    * Connection -> SSH -> Auth
* `login as: ubuntu` 로 접속

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-2-10.png"/>
</p>

### mongodb 설치 및 접속

```bash
# mongodb server 받을 준비
$ sudo apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv 0C49F3730359A14518585931BC711F9BA15703C6
$ echo "deb [ arch=amd64,arm64 ] http://repo.mongodb.org/apt/ubuntu xenial/mongodb-org/3.4 multiverse" | sudo tee /etc/apt/sources.list.d/mongodb-org-3.4.list

# 업데이트(최신버전 체크)
$ sudo apt-get update

# mongo db 다운
$ sudo apt-get install -y mongodb-org

# mongo db 실행(디폴트 포트는 27017 임)
$ sudo service mongod start
```

* EC2 27017 포트할당
    * AWS 홈페이지 -> 인스턴스 선택 -> 보안 그룹 -> 인바운드 규칙 -> 인바운드 규칙 편집

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-2-11.png"/>
</p>

* mongo db자체에서도 외부에서 접속한 것을 받도록 변경

```bash
$ sudo vi /etc/mongod.conf

# bindip: 0.0.0.0 변경
```

🐱‍🚀 **(주의)** 변경 후 서비스 -> MongoDB 재시작 해줘야함<br>

* 접속해보자 -> [Robo 3R(Robo-Mongo) 🌍](https://robomongo.org/download) (Studio 말고 그냥 3T 다운 받을 것)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-2-12.png"/>
</p>

```bash
# 만약 접속이 안된다면
$ sudo vi /etc/mongod.conf
# bindip: 0.0.0.0 가 변경되었는지 확인해 볼 필요가 있음

$ sudo vi /var/log/mongodb/mongod.log mongod.log
# 에서 bindip 확인
```

