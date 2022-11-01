---
layout: default
title: "05. 다양한 의존관계 주입 방법"
parent: "(기초)"
grand_parent: "Spring 🐍"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* 네 가지 방법이 있다
    * 생성자 주입 
    * 수정자 주입(setter 주입) 
    * 필드 주입 
    * 일반 메서드 주입

* 보면 알겠지만 사실상 생성자 주입이 가장 효과적 ...

---

## 생성자 주입

```java
// Bean으로 등록
@Component
public class OrderServiceImpl implements OrderService {

    // final 키워드에 집중 -> 최초선언만 가능
    private final MemberRepository memberRepository;
    private final DiscountPolicy discountPolicy;

    @Autowired
    // 생성자 주입이다.
    // 생성자가 하나일 경우 Autowired를 생략해도 무방
    public OrderServiceImpl(MemberRepository memberRepository, DiscountPolicy discountPolicy) {
        this.memberRepository = memberRepository;
        this.discountPolicy = discountPolicy;
    }

     // ...
```

---

## 수정자 주입(setter 주입)

```java
@Component
public class OrderServiceImpl implements OrderService 
{
    @Autowired
    public void setMemberRepository(MemberRepository memberRepository) {
        this.memberRepository = memberRepository;
    }

    @Autowired
    public void setDiscountPolicy(DiscountPolicy discountPolicy) {
        this.discountPolicy = discountPolicy;
    }

    private MemberRepository memberRepository;
    private DiscountPolicy discountPolicy;

    // ...
```

---

## 필드 주입

```java
@Component
public class OrderServiceImpl implements OrderService 
{
    @Autowired
    private MemberRepository memberRepository;
    @Autowired
    private DiscountPolicy discountPolicy;

    // ...
```

---

## 일반 메서드 주입

```java
@Component
public class OrderServiceImpl implements OrderService 
{    
    private MemberRepository memberRepository;
    private DiscountPolicy discountPolicy;

    @Autowired
    public void init(MemberRepository memberRepository, DiscountPolicy discountPolicy) {
        this.memberRepository = memberRepository;
        this.discountPolicy = discountPolicy;
    }

    // ...
```