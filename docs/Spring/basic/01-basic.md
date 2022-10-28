---
layout: default
title: "01. Spring을 사용하지 않고 Java로만 구현해보기"
parent: "(기초)"
grand_parent: "Spring 🐍"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 자주사용되는 단축키

* Settings - Ctrl + Alt + s
* generate(constructor, getter, setter) - alt + insert
* go to define - ctrl + alt + b
* navigate - ctrl + n
* Unit Test - ctrl + shift + t

* 단축키 검색법

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/basic/basic-1-3.png"/>
</p>

---

## 프로젝트 세팅

* [Clone Code 🌎](https://github.com/EasyCoding-7/spring_basic/tree/1)

* 준비물 
    * 최신 java
    * 최신 IntelliJ
* https://start.spring.io 에 접속

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/basic/basic-1-1.png"/>
</p>

* File -> Settings (Ctrl + Alt + s) -> gradle 검색 -> build and run 수정(gradle보다 intellij가 빠름.)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/basic/basic-1-2.png"/>
</p>

---

대략 만들고자 하는 것은...

* 회원
    * 회원을 가입하고 조회할 수 있다.
    * 회원은 일반과 VIP 두 가지 등급이 있다.
    * 회원 데이터는 자체 DB를 구축할 수 있고, 외부 시스템과 연동할 수 있다. (미확정)
* 주문과 할인 정책
    * 회원은 상품을 주문할 수 있다.
    * 회원 등급에 따라 할인 정책을 적용할 수 있다.
    * 할인 정책은 모든 VIP는 1000원을 할인해주는 고정 금액 할인을 적용해달라. (나중에 변경 될 수
    있다.)
    * 할인 정책은 변경 가능성이 높다. 회사의 기본 할인 정책을 아직 정하지 못했고, * 오픈 직전까지 고민을 미루고 싶다. 최악의 경우 할인을 적용하지 않을 수 도 있다. (미확정)

---

## 회원 구현

* [Clone Code 🌎](https://github.com/EasyCoding-7/spring_basic/tree/2)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/basic/basic-1-4.png"/>
</p>

```java
package hello.core.member;

public enum Grade {
    BASIC,
    VIP
}
```

```java
package hello.core.member;

public class Member {
    private Long id;
    private String name;
    private Grade grade;

    public Member(Long id, String name, Grade grade) {
        this.id = id;
        this.name = name;
        this.grade = grade;
    }

    public Long getId() {
        return id;
    }

    public void setId(Long id) {
        this.id = id;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public Grade getGrade() {
        return grade;
    }

    public void setGrade(Grade grade) {
        this.grade = grade;
    }
}
```

```java
package hello.core.member;

public interface MemberRepository {
    void save(Member member);
    Member findById(Long memberId);
}
```

```java
package hello.core.member;

import java.util.HashMap;
import java.util.Map;

public class MemoryMemberRepository implements MemberRepository{
    private static Map<Long, Member> store = new HashMap<>();

    @Override
    public void save(Member member) {
        store.put(member.getId(), member);
    }

    @Override
    public Member findById(Long memberId) {
        return store.get(memberId);
    }
}
```

---

## 회원 서비스

* [Clone Code 🌎](https://github.com/EasyCoding-7/spring_basic/tree/3)

```java
package hello.core.member;

public interface MemberService {

    void join(Member member);

    Member findMember(Long memberId);
}
```

```java
package hello.core.member;

public class MemberServiceImpl implements MemberService {
    private final MemberRepository memberRepository = new MemoryMemberRepository();

    @Override
    public void join(Member member) {
        memberRepository.save(member);
    }

    @Override
    public Member findMember(Long memberId) {
        return memberRepository.findById(memberId);
    }
}
```

---

## UnitTest적용

* [Clone Code 🌎](https://github.com/EasyCoding-7/spring_basic/tree/4)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/basic/basic-1-5.png"/>
</p>

```java
package hello.core.member;

import org.assertj.core.api.Assertions;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.*;

class MemberServiceTest {
    MemberService memberService = new MemberServiceImpl();

    @Test
    void join() {
        //given
        Member member = new Member(1L, "memberA", Grade.VIP);
        //when
        memberService.join(member);
        Member findMember = memberService.findMember(1L);
        //then
        Assertions.assertThat(member).isEqualTo(findMember);
    }
}
```

---

## 주문 도메인

* [Clone Code 🌎](https://github.com/EasyCoding-7/spring_basic/tree/5)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/basic/basic-1-6.png"/>
</p>

```java
package hello.core.discount;

import hello.core.member.Member;

public interface DiscountPolicy {
    /**
     * @return 할인 대상 금액
     */
    int discount(Member member, int price);
}
```

```java
package hello.core.discount;

import hello.core.member.Grade;
import hello.core.member.Member;

public class FixDiscountPolicy implements DiscountPolicy {
    private int discountFixAmount = 1000; //1000원 할인

    @Override
    public int discount(Member member, int price) {
        if (member.getGrade() == Grade.VIP) {
            return discountFixAmount;
        } else {
            return 0;
        }
    }
}
```

```java
package hello.core.order;

public interface OrderService {
    Order createOrder(Long memberId, String itemName, int itemPrice);
}
```

```java
package hello.core.order;

import hello.core.discount.DiscountPolicy;
import hello.core.discount.FixDiscountPolicy;
import hello.core.member.Member;
import hello.core.member.MemberRepository;
import hello.core.member.MemoryMemberRepository;

public class OrderServiceImpl implements OrderService {

    private final MemberRepository memberRepository = new
            MemoryMemberRepository();
    private final DiscountPolicy discountPolicy = new FixDiscountPolicy();
    @Override
    public Order createOrder(Long memberId, String itemName, int itemPrice) {

        Member member = memberRepository.findById(memberId);
        int discountPrice = discountPolicy.discount(member, itemPrice);
        return new Order(memberId, itemName, itemPrice, discountPrice);
    }
}

```

```java
package hello.core.order;

public class Order {
    private Long memberId;
    private String itemName;
    private int itemPrice;
    private int discountPrice;

    public Order(Long memberId, String itemName, int itemPrice, int discountPrice) {
        this.memberId = memberId;
        this.itemName = itemName;
        this.itemPrice = itemPrice;
        this.discountPrice = discountPrice;
    }

    public Long getMemberId() {
        return memberId;
    }

    public void setMemberId(Long memberId) {
        this.memberId = memberId;
    }

    public String getItemName() {
        return itemName;
    }

    public void setItemName(String itemName) {
        this.itemName = itemName;
    }

    public int getItemPrice() {
        return itemPrice;
    }

    public void setItemPrice(int itemPrice) {
        this.itemPrice = itemPrice;
    }

    public int getDiscountPrice() {
        return discountPrice;
    }

    public void setDiscountPrice(int discountPrice) {
        this.discountPrice = discountPrice;
    }

    @Override
    public String toString() {
        return "Order{" +
                "memberId=" + memberId +
                ", itemName='" + itemName + '\'' +
                ", itemPrice=" + itemPrice +
                ", discountPrice=" + discountPrice +
                '}';
    }
}

```

---

## Unit Test 2

```java
package hello.core.order;

import hello.core.member.Grade;
import hello.core.member.Member;
import hello.core.member.MemberService;
import hello.core.member.MemberServiceImpl;
import org.assertj.core.api.Assertions;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.*;

class OrderServiceTest {
    MemberService memberService = new MemberServiceImpl();
    OrderService orderService = new OrderServiceImpl();

    @Test
    void createOrder() {
        long memberId = 1L;
        Member member = new Member(memberId, "memberA", Grade.VIP);
        memberService.join(member);
        Order order = orderService.createOrder(memberId, "itemA", 10000);
        Assertions.assertThat(order.getDiscountPrice()).isEqualTo(1000);
    }
}
```

---

## DiscountPolicy를 하나 더 추가해보자.

* [Clone Code 🌎](https://github.com/EasyCoding-7/spring_basic/tree/6)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/basic/basic-1-7.png"/>
</p>

```java
package hello.core.discount;

import hello.core.member.Grade;
import hello.core.member.Member;

public class RateDiscountPolicy implements DiscountPolicy {
    private int discountPercent = 10; //10% 할인
    @Override
    public int discount(Member member, int price) {
        if (member.getGrade() == Grade.VIP) {
            return price * discountPercent / 100;
        } else {
            return 0;
        }
    }
}
```

```java
package hello.core.discount;

import hello.core.member.Grade;
import hello.core.member.Member;
import org.junit.jupiter.api.DisplayName;
import org.junit.jupiter.api.Test;

import static org.assertj.core.api.Assertions.*;

class RateDiscountPolicyTest {
    RateDiscountPolicy discountPolicy = new RateDiscountPolicy();
    @Test
    @DisplayName("VIP는 10% 할인이 적용되어야 한다.")
    void vip_o() {
        //given
        Member member = new Member(1L, "memberVIP", Grade.VIP);
        //when
        int discount = discountPolicy.discount(member, 10000);
        //then
        assertThat(discount).isEqualTo(1000);
    }
    @Test
    @DisplayName("VIP가 아니면 할인이 적용되지 않아야 한다.")
    void vip_x() {
        //given
        Member member = new Member(2L, "memberBASIC", Grade.BASIC);
        //when
        int discount = discountPolicy.discount(member, 10000);
        //then
        assertThat(discount).isEqualTo(0);
    }
}
```

---

## 기존코드의 문제점 ???

* [Clone Code 🌎](https://github.com/EasyCoding-7/spring_basic/tree/7)

```java
public class OrderServiceImpl implements OrderService{
    private final MemberRepository memberRepository = new MemoryMemberRepository();

    // 인스턴스를 하드코딩해야한다.
    private final DiscountPolicy discountPolicy = new FixDiscountPolicy();

    // ...
```

* 관심사를 분리함으로서 해결해 보자. -> appconfig구현

```java
package hello.core;

import hello.core.discount.DiscountPolicy;
import hello.core.discount.FixDiscountPolicy;
import hello.core.member.MemberRepository;
import hello.core.member.MemberService;
import hello.core.member.MemberServiceImpl;
import hello.core.member.MemoryMemberRepository;
import hello.core.order.OrderService;
import hello.core.order.OrderServiceImpl;

public class AppConfig {
    public MemberService memberService() {
        return new MemberServiceImpl(memberRepository());
    }
    public OrderService orderService() {
        return new OrderServiceImpl(
                memberRepository(),
                discountPolicy());
    }
    public MemberRepository memberRepository() {
        return new MemoryMemberRepository();
    }
    public DiscountPolicy discountPolicy() {
        return new FixDiscountPolicy();
    }
}

```
