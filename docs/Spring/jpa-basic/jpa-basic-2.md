---
layout: default
title: "02. Psersistence Context(영속성 컨텍스트)"
parent: "(JPA BASIC)"
grand_parent: "Spring 🐍"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Clone Code 🌎](https://github.com/EasyCoding-7/ex1-hello-jpa/tree/4)

* jpa에서 persist()를 한다해서 db에 바로 저장되는것이 아니다.
* 우선 영속성 컨텍스트에 저장 후  commit이후에  db에 반영이 된다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/jpa-basic/jpa-basic-2-1.png"/>
</p>

---

## Persistence Context에는 네 가지 상태가 있다.

* 비영속 (new/transient) 
    * 영속성 컨텍스트와 전혀 관계가 없는 새로운 상태 

```java
//객체를 생성한 상태(비영속)
    // jpa와 관계없이 객체만 생성한 상태를 의미
Member member = new Member();
member.setId("member1");
member.setUsername("회원1");
```

* 영속 (managed) 
    * 영속성 컨텍스트에 관리되는 상태 

```java
//객체를 생성한 상태(비영속)
Member member = new Member();
member.setId("member1");
member.setUsername(“회원1”);
EntityManager em = emf.createEntityManager();
em.getTransaction().begin();
//객체를 저장한 상태(영속)
em.persist(member);
```

* 준영속 (detached) 
    * 영속성 컨텍스트에 저장되었다가 분리된 상태 

```java
//회원 엔티티를 영속성 컨텍스트에서 분리, 준영속 상태
em.detach(member);
```

* 삭제 (removed) 
    * 삭제된 상태

```java
//객체를 삭제한 상태(삭제)
em.remove(member);
```

---

## 영속성 컨텍스트의 이점 

* 1차 캐시

```java
Member member = new Member();
member.setId("member1");
member.setUsername("회원1");
//1차 캐시에 저장됨
em.persist(member);
//1차 캐시에서 조회
Member findMember = em.find(Member.class, "member1");
```

* 동일성(identity) 보장 

```java
Member a = em.find(Member.class, "member1");
Member b = em.find(Member.class, "member1");
System.out.println(a == b); //동일성 비교 true
```

* 트랜잭션을 지원하는 쓰기 지연 (transactional write-behind)

```java
EntityManager em = emf.createEntityManager();
EntityTransaction transaction = em.getTransaction();
//엔티티 매니저는 데이터 변경시 트랜잭션을 시작해야 한다.
transaction.begin(); // [트랜잭션] 시작
em.persist(memberA);
em.persist(memberB);
//여기까지 INSERT SQL을 데이터베이스에 보내지 않는다.
//커밋하는 순간 데이터베이스에 INSERT SQL을 보낸다.
transaction.commit(); // [트랜잭션] 커밋
```

* 변경 감지(Dirty Checking)

```java
EntityManager em = emf.createEntityManager();
EntityTransaction transaction = em.getTransaction();
transaction.begin(); // [트랜잭션] 시작
// 영속 엔티티 조회
Member memberA = em.find(Member.class, "memberA");
// 영속 엔티티 데이터 수정
memberA.setUsername("hi");
memberA.setAge(10);
//em.update(member) 이런 코드가 있어야 하지 않을까?
// 변경이 감지되기에 영속성 컨텍스트가 알아서 해준다
transaction.commit(); // [트랜잭션] 커밋
```

```java
//삭제 대상 엔티티 조회
Member memberA = em.find(Member.class, “memberA");
em.remove(memberA); //엔티티 삭제
```

---

## 플러시 ? 

* 변경 감지 
* 수정된 엔티티 쓰기 지연 SQL 저장소에 등록 
* 쓰기 지연 SQL 저장소의 쿼리를 데이터베이스에 전송 (등록, 수정, 삭제 쿼리)

---

## 영속성 컨텍스트를 플러시하는 방법

* em.flush() - 직접 호출 
* 트랜잭션 커밋 - 플러시 자동 호출 
* JPQL 쿼리 실행 - 플러시 자동 호출