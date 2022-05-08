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

## Configuration

🍒 스프링 컨테이너의 생성과정

```java
public class MemberApp {
    public static void main(String[] args) {
        ApplicationContext applicationContext = new
                AnnotationConfigApplicationContext(AppConfig.class);

        // ...
```

```java
// 참고로 AppConfig는 @Configuration 기반이다.
@Configuration
public class AppConfig {
    // ...
```

🍒 여기까지하면 스프링은 빈 컨테이너를 생성<br>
🍒 빈 컨테이너 아래 Bean을 넣는다.

```java
@Configuration
public class AppConfig {
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

🍒 컨테이너에 빈이 들어가고 이후 서로의 의존관계를 등록하게된다.<br>
🍒 눈으로 확인할 수 없어 말로 설명해야하지만 대략 그렇다고 이해하자.

---

## Bean

🍒 간단한 테스트로 컨테이너에 등록된 빈을 조회해 보자.

* [Get Code 🌍](https://github.com/EasyCoding-7/spring-basic-tutorial/tree/2-2)

```java
class ApplicationContextInfoTest {
    AnnotationConfigApplicationContext ac = new
            AnnotationConfigApplicationContext(AppConfig.class);

    @Test
    @DisplayName("모든 빈 출력하기")    // 스프링 내부 빈 까지 모두 출력된다.
    void findAllBean() {
        String[] beanDefinitionNames = ac.getBeanDefinitionNames();
        for (String beanDefinitionName : beanDefinitionNames) {
            Object bean = ac.getBean(beanDefinitionName);
            System.out.println("name=" + beanDefinitionName + " object=" +
                    bean);
        }
    }

    @Test
    @DisplayName("애플리케이션 빈 출력하기")
    void findApplicationBean() {
        String[] beanDefinitionNames = ac.getBeanDefinitionNames();
        for (String beanDefinitionName : beanDefinitionNames) {
            BeanDefinition beanDefinition =
                    ac.getBeanDefinition(beanDefinitionName);
            //Role ROLE_APPLICATION: 직접 등록한 애플리케이션 빈
            //Role ROLE_INFRASTRUCTURE: 스프링이 내부에서 사용하는 빈
            if (beanDefinition.getRole() == BeanDefinition.ROLE_APPLICATION) {
                Object bean = ac.getBean(beanDefinitionName);
                System.out.println("name=" + beanDefinitionName + " object=" +
                        bean);
            }
        }
    }
}
```

🍒 추가적으로 빈 조회하는 법

```java
class ApplicationContextBasicFindTest {
    AnnotationConfigApplicationContext ac = new
            AnnotationConfigApplicationContext(AppConfig.class);

    @Test
    @DisplayName("빈 이름으로 조회")
    void findBeanByName() {
        MemberService memberService = ac.getBean("memberService",
                MemberService.class);
        assertThat(memberService).isInstanceOf(MemberServiceImpl.class);
    }

    @Test
    @DisplayName("이름 없이 타입만으로 조회")
    void findBeanByType() {
        MemberService memberService = ac.getBean(MemberService.class);
        assertThat(memberService).isInstanceOf(MemberServiceImpl.class);
    }

    @Test
    @DisplayName("구체 타입으로 조회")
    void findBeanByName2() {
        MemberServiceImpl memberService = ac.getBean("memberService",
                MemberServiceImpl.class);
        assertThat(memberService).isInstanceOf(MemberServiceImpl.class);
    }

    @Test
    @DisplayName("빈 이름으로 조회X")
    void findBeanByNameX() {
        //ac.getBean("xxxxx", MemberService.class);
        Assertions.assertThrows(NoSuchBeanDefinitionException.class, () ->
                ac.getBean("xxxxx", MemberService.class));
    }
}
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/basic/2-1.png"/>
</p>

{% endraw %}