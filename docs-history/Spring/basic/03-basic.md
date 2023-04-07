---
layout: default
title: "03. singleton 컨테이너"
parent: "(기초)"
grand_parent: "Spring 🐍"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Clone Code 🌎](https://github.com/EasyCoding-7/spring_basic/tree/15)

* 이런상황을 가정해보자. Client가 동시에 여러명 접속하여 Request를 보낸다.
* 우선 순수 java로 처리하자면 대략 아래와 같다.

```java
package hello.core.singleton;

import hello.core.AppConfig;
import hello.core.member.MemberService;
import org.junit.jupiter.api.DisplayName;
import org.junit.jupiter.api.Test;

import static org.assertj.core.api.AssertionsForClassTypes.assertThat;

public class SingletonTest {
    @Test
    @DisplayName("스프링 없는 순수한 DI 컨테이너")
    void pureContainer() {
        AppConfig appConfig = new AppConfig();
        //1. 조회: 호출할 때 마다 객체를 생성
        MemberService memberService1 = appConfig.memberService();
        //2. 조회: 호출할 때 마다 객체를 생성
        MemberService memberService2 = appConfig.memberService();
        //참조값이 다른 것을 확인
        System.out.println("memberService1 = " + memberService1);
        System.out.println("memberService2 = " + memberService2);
        //memberService1 != memberService2
        assertThat(memberService1).isNotSameAs(memberService2);
    }
}
```

* 그런데 `MemberService`가 각 Client마다 필요할까? 하나만 만들어 보면 어떨까?

```java
package hello.core.singleton;

public class SingletonService {
    //1. static 영역에 객체를 딱 1개만 생성해둔다.
    private static final SingletonService instance = new SingletonService();
    //2. public으로 열어서 객체 인스터스가 필요하면 이 static 메서드를 통해서만 조회하도록 허용한다.
    public static SingletonService getInstance() {
        return instance;
    }
    //3. 생성자를 private으로 선언해서 외부에서 new 키워드를 사용한 객체 생성을 못하게 막는다.
    private SingletonService() {
    }
    public void logic() {
        System.out.println("싱글톤 객체 로직 호출");
    }
}
```

```java
@Test
@DisplayName("싱글톤 패턴을 적용한 객체 사용")
public void singletonServiceTest() {
    //private으로 생성자를 막아두었다. 컴파일 오류가 발생한다.
    //new SingletonService();
    //1. 조회: 호출할 때 마다 같은 객체를 반환
    SingletonService singletonService1 = SingletonService.getInstance();
    //2. 조회: 호출할 때 마다 같은 객체를 반환
    SingletonService singletonService2 = SingletonService.getInstance();
    //참조값이 같은 것을 확인
    System.out.println("singletonService1 = " + singletonService1);
    System.out.println("singletonService2 = " + singletonService2);
    // singletonService1 == singletonService2
    assertThat(singletonService1).isSameAs(singletonService2);
    singletonService1.logic();
}
```

---

## singleton의 문제점?

* [Clone Code 🌎](https://github.com/EasyCoding-7/spring_basic/tree/15)

* 결론적으로 말해서 singleton은 상태를 갖으면 안된다.

```java
package hello.core.singleton;

import org.assertj.core.api.Assertions;
import org.junit.jupiter.api.Test;
import org.springframework.context.ApplicationContext;
import org.springframework.context.annotation.AnnotationConfigApplicationContext;
import org.springframework.context.annotation.Bean;

import static org.junit.jupiter.api.Assertions.*;

class StatefulServiceTest {
    @Test
    void statefulServiceSingleton() {
        ApplicationContext ac = new
                AnnotationConfigApplicationContext(TestConfig.class);
        StatefulService statefulService1 = ac.getBean("statefulService",
                StatefulService.class);
        StatefulService statefulService2 = ac.getBean("statefulService",
                StatefulService.class);
        //ThreadA: A사용자 10000원 주문
        statefulService1.order("userA", 10000);
        //ThreadB: B사용자 20000원 주문
        statefulService2.order("userB", 20000);
        //ThreadA: 사용자A 주문 금액 조회
        int price = statefulService1.getPrice();
        //ThreadA: 사용자A는 10000원을 기대했지만, 기대와 다르게 20000원 출력
        System.out.println("price = " + price);
        Assertions.assertThat(statefulService1.getPrice()).isEqualTo(20000);
    }
    static class TestConfig {
        @Bean
        public StatefulService statefulService() {
            return new StatefulService();
        }
    }
}
```

---

## @Configuration과 Singleton

* 의문점이 있다.

```java
// Appconfig.java 에서 아래와 같이 memberServier()를 호출시
// 매번 new를 해서 object를 새로 생성하게 되나???
@Configuration
public class AppConfig {
    @Bean
    public MemberService memberService() {
        return new MemberServiceImpl(memberRepository());
    }
```

* 결론부터 말하자면 `@Configuration`에 의해서 Singleton이 유지된다.
* 어떻게 하는 것인가?? -> 아래참조!

### @Configuration과 바이트코드 조작

* 실제로 생성한 bean은 내가 생성한 bean이 아니다??!?

```java
ApplicationContext ac = new
AnnotationConfigApplicationContext(AppConfig.class);
//AppConfig도 스프링 빈으로 등록된다.
AppConfig bean = ac.getBean(AppConfig.class);

// class hello.core.AppConfig가 아닌
// class hello.core.AppConfig@CGLIB가 출력된다.
System.out.println("bean = " + bean.getClass());
// Spring이 내부적으로 클래스를 조작!
// 그 조작된 클래스가 싱글톤이되게 스프링이 보장해주는것이다.
```

* 만약 `@Configuration`을 넣지 않는다면???
* Spring은 Singleton을 보장하지 않게된다.