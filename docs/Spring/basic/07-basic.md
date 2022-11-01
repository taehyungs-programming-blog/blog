---
layout: default
title: "07. Autowired시에 조회되는 Bean이 2개 이상이라면?"
parent: "(기초)"
grand_parent: "Spring 🐍"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```java
// Bean이 아래와 같고
@Component
public class FixDiscountPolicy implemets DiscountPolicy {}

@Component
public class RateDiscountPolicy implemets DiscountPolicy {}

// 사용하는 입장에서

@Autowired
private DiscountPolicy discountPolicy
// -> 이렇게 써버린다면??
// -> 에러발생
```

---

* 해결하기 위한, 세 가지 방법이 존재한다.
    * @Autowired 필드 명 매칭 
    * @Qualifier @Qualifier끼리 매칭 빈 이름 매칭 
    * @Primary 사용

---

## @Autowired 필드 명 매칭

```java
@Autowired 필드 명 매칭 
@Autowired 
private DiscountPolicy rateDiscountPolicy
// 이름을 보고 따라간다
```

---

## @Qualifier @Qualifier끼리 매칭 빈 이름 매칭

```java
@Component
@Qualifier("mainDiscountPolicy")
public class RateDiscountPolicy implements DiscountPolicy {}
```

```java
@Component
@Qualifier("fixDiscountPolicy")
public class FixDiscountPolicy implements DiscountPolicy {}
```

```java
@Autowired
public OrderServiceImpl(MemberRepository memberRepository,
                        @Qualifier("mainDiscountPolicy") DiscountPolicy
                                discountPolicy) {
    this.memberRepository = memberRepository;
    this.discountPolicy = discountPolicy;
}
```

---

## @Primary 사용

```java
@Component
@Primary
public class RateDiscountPolicy implements DiscountPolicy {}
@Component
public class FixDiscountPolicy implements DiscountPolicy {}
```

---

## Annotation 직접 만들기

* @Qualifier("mainDiscountPolicy") 이렇게 문자를 적으면 컴파일시 타입 체크가 안된다. 
* 다음과 같은 애노테이션을 만들어서 문제를 해결할 수 있다.

```java
package hello.core.annotataion;

import org.springframework.beans.factory.annotation.Qualifier;

import java.lang.annotation.*;

@Target({ElementType.FIELD, ElementType.METHOD, ElementType.PARAMETER, ElementType.TYPE, ElementType.ANNOTATION_TYPE})
@Retention(RetentionPolicy.RUNTIME)
@Documented
@Qualifier("mainDiscountPolicy")
public @interface MainDiscountPolicy {
}
```

```java
@Component
@MainDiscountPolicy
public class RateDiscountPolicy implements DiscountPolicy {}
```