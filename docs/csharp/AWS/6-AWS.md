---
layout: default
title: "6. 각종 서비스"
parent: (AWS)
grand_parent: "(C#)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## S3(Simple Service Storage)

* 이름 그대로 저장소를 제공

---

## RDS

* 데이터베이스
* EC2에 그냥 설치하면 안되나?
    * 라이선스등의 문제로 위험할 수 있음. RDS를 통해 관리하는것이 오히려깔끔.

---

## Route 53

* EC2를 껏다 키거나 할 경우 **public ip가 변경**되게 된다.
* 물론 **탄력적 ip**를 사용해도 됨
* 여기서 또 다른 문제는 매번 ip로 접속해야 하나이다
* **Route 53**의 등장 - DNS서비스를 AWS에서 해준다

---

## ELB

* **Elastic Load Balanacer** - 부하 분산용으로 사용된다.
* 잉? Route 53과 무슨차이인가? - Route 53은 부하를 분산해 주진 못한다.

---

## Auto Scaling

---

## IAM

* **Identity and Access Management** - 보안을 담당한다.
