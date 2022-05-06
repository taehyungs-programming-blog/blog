---
layout: default
title: "2. 스프링 컨테이너, 스프링 빈"
parent: (spring - 초급)
grand_parent: C++
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

{% raw %}

🍒 들어가기 전 용어정리 부터

## IoC, DI, 컨테이너

* **IoC**(Inversion of Control) : 제어의 역전
    * 내가 내 코드를 호출하는 것이 아니라 프레임워크에 의해 호출당하는 것을 말한다.
    * 우리가 만든 예제로 설명하자면, AppConfig에서 모든 클래스를 생성하고 각 클래스는 서로가 어느 타이밍에 호출되는지 생성되는지는 모르게 된다.(일종의 IoC가 된다.)
* **DI**(Dependency Injection) : 의존관계 주입
* **컨테이너**
    * AppConfig를 IoC컨테이너, DI컨테이너라 부른다.

---

## 스프링으로 전화하기

* [Get Code 🌍](https://github.com/EasyCoding-7/spring-basic-tutorial/tree/2-1)

```java
// 컨테이너에 @Configuration를 붙여준다.
@Configuration
public class AppConfig {
    // 내부의 함수에 각각 @Bean을 붙여준다.
    @Bean
    public MemberService memberService() {
        return new MemberServiceImpl(memberRepository());
    }
    @Bean
    public OrderService orderService() {
        return new OrderServiceImpl(
                memberRepository(),
                discountPolicy());
    }
    @Bean
    public MemberRepository memberRepository() {
        return new MemoryMemberRepository();
    }
    @Bean
    public DiscountPolicy discountPolicy() {
        return new RateDiscountPolicy();
    }
}
```

🍒 여기서 설명보단 어떻게 쓰이는지 보는게 더 정확하다.

```java
public class MemberApp {
    public static void main(String[] args) {
// AppConfig appConfig = new AppConfig();
// MemberService memberService = appConfig.memberService();
        ApplicationContext applicationContext = new
                AnnotationConfigApplicationContext(AppConfig.class);
        MemberService memberService =
                applicationContext.getBean("memberService", MemberService.class);
        Member member = new Member(1L, "memberA", Grade.VIP);
        memberService.join(member);
        Member findMember = memberService.findMember(1L);
        System.out.println("new member = " + member.getName());
        System.out.println("find Member = " + findMember.getName());
    }
}
```

🍒 대략 사용법은 알겠는데... 이게 과연 장접이 있는가?(아래서 설명)

---

##

* [Get Code 🌍](https://github.com/EasyCoding-7/spring-basic-tutorial/tree/2-2)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/basic/2-1.png"/>
</p>

{% endraw %}