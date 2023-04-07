---
layout: default
title: "04. 컴포넌트 스캔"
parent: "(기초)"
grand_parent: "Spring 🐍"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 컴포넌트 스캔과 의존관계 주입

* [Clone Code 🌎](https://github.com/EasyCoding-7/spring_basic/tree/16)

* 수백수천개의 bean을 appconfig코드로 등록해야할까?
* 좀 더 편하게 할 방법은?? -> **컴포넌트 스캔**

```java
package hello.core;

import org.springframework.context.annotation.ComponentScan;
import org.springframework.context.annotation.Configuration;
import org.springframework.context.annotation.FilterType;

@Configuration
@ComponentScan(
        // 뺄것을 지정
        excludeFilters = @ComponentScan.Filter(type = FilterType.ANNOTATION, classes = Configuration.class)
        // Configuration.class는 제외해주세요
        // -> @Configuration으로 수동등록할 애들은 빼기위함.
        )
public class AutoAppConfig {

}
```

```java
@Component
public class MemoryMemberRepository implements MemberRepository{
    // ...
```

```java
@Component
public class RateDiscountPolicy implements DiscountPolicy {
    // ...
```

```java
@Component
public class MemberServiceImpl implements MemberService {
    private final MemberRepository memberRepository;

    @Autowired
    public MemberServiceImpl(MemberRepository memberRepository) {
        this.memberRepository = memberRepository;
    }
    // ...
```

```java
@Component
public class OrderServiceImpl implements OrderService {

    private final MemberRepository memberRepository;
    private final DiscountPolicy discountPolicy;
    
    @Autowired
    public OrderServiceImpl(MemberRepository memberRepository, DiscountPolicy
            discountPolicy) {
        this.memberRepository = memberRepository;
        this.discountPolicy = discountPolicy;
    }
    // ...
```

* 테스트해보자

```java
package hello.core.scan;

import hello.core.AutoAppConfig;
import hello.core.member.MemberService;
import org.junit.jupiter.api.Test;
import org.springframework.context.ApplicationContext;
import org.springframework.context.annotation.AnnotationConfigApplicationContext;

import static org.assertj.core.api.AssertionsForClassTypes.assertThat;

public class AutoAppConfigTest {
    @Test
    void basicScan() {
        ApplicationContext ac = new
                AnnotationConfigApplicationContext(AutoAppConfig.class);
        MemberService memberService = ac.getBean(MemberService.class);
        assertThat(memberService).isInstanceOf(MemberService.class);
    }
}
```

---

## 탐색 위치와 기본 스캔대상 지정

```java
@ComponentScan(
    // 탐색할 패키지의 시작 위치 지정
    basePackages = "hello.core",
    // ...

    // 만약 탐색할 패키지를 지정하지 않는다면??
    // @ComponentScan을 호출한 패키지를 basePackages로 둔다
        // AppConfig.java를 hello.core최상단에 둔 이유이다
```

* 컴포넌트 스캔 기본 대상
    * 컴포넌트 스캔은 @Component 뿐만 아니라 다음과 내용도 추가로 대상에 포함한다.
    * `@Component` : 컴포넌트 스캔에서 사용
    * `@Controlller` : 스프링 MVC 컨트롤러에서 사용
    * `@Service` : 스프링 비즈니스 로직에서 사용
    * `@Repository` : 스프링 데이터 접근 계층에서 사용
    * `@Configuration` : 스프링 설정 정보에서 사용

---

## 필터

* [Clone Code 🌎](https://github.com/EasyCoding-7/spring_basic/tree/17)

```java
package hello.core.scan.filter;

import java.lang.annotation.*;

@Target(ElementType.TYPE)
@Retention(RetentionPolicy.RUNTIME)
@Documented
public @interface MyIncludeComponent {
}
```

```java
package hello.core.scan.filter;

import java.lang.annotation.*;

@Target(ElementType.TYPE)
@Retention(RetentionPolicy.RUNTIME)
@Documented
public @interface MyExcludeComponent {
}
```

```java
package hello.core.scan.filter;

@MyIncludeComponent
public class BeanA {
}
```

```java
package hello.core.scan.filter;

@MyExcludeComponent
public class BeanB {
}
```

```java
package hello.core.scan.filter;

import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;
import org.springframework.beans.factory.NoSuchBeanDefinitionException;
import org.springframework.context.ApplicationContext;
import org.springframework.context.annotation.AnnotationConfigApplicationContext;
import org.springframework.context.annotation.ComponentScan;
import org.springframework.context.annotation.Configuration;
import org.springframework.context.annotation.FilterType;

import static org.assertj.core.api.AssertionsForClassTypes.assertThat;

public class ComponentFilterAppConfigTest {
    @Test
    void filterScan() {
        ApplicationContext ac = new
                AnnotationConfigApplicationContext(ComponentFilterAppConfig.class);
        BeanA beanA = ac.getBean("beanA", BeanA.class);
        assertThat(beanA).isNotNull();
        Assertions.assertThrows(
                NoSuchBeanDefinitionException.class,
                () -> ac.getBean("beanB", BeanB.class));
    }
    @Configuration
    @ComponentScan(
            includeFilters = @ComponentScan.Filter(type = FilterType.ANNOTATION, classes =
                    MyIncludeComponent.class),
            excludeFilters = @ComponentScan.Filter(type = FilterType.ANNOTATION, classes =
                    MyExcludeComponent.class)
    )
    static class ComponentFilterAppConfig {
    }
}
```

