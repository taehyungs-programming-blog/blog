---
layout: default
title: "5. VPC(Virtual Private Cloud)"
parent: (AWS)
grand_parent: C#
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 이론

* **VPC** - 일종의 사내망이라 생각하자, 보안관리의 편의성을 위해서 사내망을 관리
    * **ACL** - 사내망과 외부망의 보안을 담당
    * **보안그룹** - 사내망의 각 PC의 보안을 담당
* **Private/Public** 주소
    * **Private** - 사내망에서 사용되는 주소
    * **Public** - 사외망에서 사용될 주소
* **Subnet** - VPC내의 VPC라 생각하면 된다

---

## 실습

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/aws/aws-5-1.png"/>
</p>

* **VPC 마법사**를 통해서 VPC를 만들어 보자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/aws/aws-5-2.png"/>
</p>

* **CIDR** - VPC내부에서 사용될 Private 주소에 쓰일 주소를 의미

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/aws/aws-5-3.png"/>
</p>

* **가용영역** - 네트워크의 안정성을 위해 특정지역에만 VPC를 만드는게 아니라 여러지역에 나눠서 VPC를 만들수 있음

