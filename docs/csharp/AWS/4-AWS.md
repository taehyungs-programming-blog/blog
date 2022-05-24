---
layout: default
title: "4. EC2"
parent: (AWS)
grand_parent: C#
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Windows

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/aws/aws-4-1.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/aws/aws-4-2.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/aws/aws-4-3.png"/>
</p>

😀 연결을 위해서 **.pem**이 필요하니 잘 관리하자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/aws/aws-4-4.png"/>
</p>

😀 인스턴스 우클릭 후 **연결**클릭

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/aws/aws-4-5.png"/>
</p>

😀 **암호 가져오기** 클릭

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/aws/aws-4-6.png"/>
</p>

😀 **.pem**를 넣자, 이제 RDP 접속이 가능해 진다.

---

## Linux

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/aws/aws-4-7.png"/>
</p>

😀 이번엔 **putty**로 접속할 예정이고 **.pem**을 putty가 쓸수 있게 번형해 줘야한다.<br>
😀 **puttygen.exe** 실행

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/aws/aws-4-8.png"/>
</p>

😀 Conversions -> Import Key 선택 후 **.pem** 선택<br>
😀 **Save private key** 클릭 후 저장<br>
😀 putty에서 Auth -> Private Key를 넣어준다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/aws/aws-4-9.png"/>
</p>

😀 초기 아이디는 **ec2-user**이다.