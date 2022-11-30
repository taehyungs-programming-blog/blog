---
layout: default
title: "08. AOP(Aspect Oriented Programming)"
parent: "(입문)"
grand_parent: "Spring 🐍"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## AOP가 필요한 상황 

- 모든 메소드의 호출 시간을 측정하고 싶다면? 
- 모든 메소드에 시간측정 로직을 추가해야할까?

* **공통 관심 사항(cross-cutting concern) vs 핵심 관심 사항(core concern) 분리**

---

```java
@Component      // Spring bean입니다.(보통 Java Style로 @Bean 등록함)
@Aspect         // AOP로 사용하겠다. 알림
public class TimeTraceAop {
    @Around("execution(* hello.hellospring..*(..))")
    // 어디에 AOP를 적용할지 알림 hello.hellospring 하위에 적용해주세요
    public Object execute(ProceedingJoinPoint joinPoint) throws Throwable {
        long start = System.currentTimeMillis();
        System.out.println("START: " + joinPoint.toString());
        try {
            return joinPoint.proceed();
        } finally {
            long finish = System.currentTimeMillis();
            long timeMs = finish - start;
            System.out.println("END: " + joinPoint.toString()+ " " + timeMs + "ms");
        }
    }
}
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/entry/entry-8-1.png"/>
</p>