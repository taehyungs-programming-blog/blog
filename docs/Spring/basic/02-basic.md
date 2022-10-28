---
layout: default
title: "02. spring 적용해 보기"
parent: "(기초)"
grand_parent: "Spring 🐍"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Spring bean을 만들어 보자

* [Clone Code 🌎](https://github.com/EasyCoding-7/spring_basic/tree/8)

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
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

@Configuration
public class AppConfig {
    // 인스턴스를 Bean으로 지정

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
        return new FixDiscountPolicy();
    }
}

```

```java
package hello.core;

// ...

@SpringBootApplication
public class CoreApplication {

	public static void main(String[] args) {

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

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/basic/basic-2-1.png"/>
</p>


* 아직은 뭐가 좋은진 잘 모르겠으나 ... bean으로 추가됬음을 확인가능하다.

---