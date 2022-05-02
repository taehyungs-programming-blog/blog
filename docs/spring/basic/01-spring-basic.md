---
layout: default
title: "1. 프로젝트 생성, 순수 Java만 써서 웹서비스 만들어 보기"
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

## 사전 준비

* [JAVA 11 🌍](https://www.oracle.com/kr/java/technologies/javase/jdk11-archive-downloads.html)
* [IntelliJ 🌍](https://www.jetbrains.com/ko-kr/idea/download/download-thanks.html?platform=windows&code=IIC)

---

## 프로젝트 세팅

* [Get Code 🌍](https://github.com/EasyCoding-7/spring-basic-tutorial/tree/1-1)

🐳 `https://start.spring.io/` 에 접속한다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/basic/1-1.png"/>
</p>

🐳 별도로 변경한 부분만 마킹해두었으니 참고<br>
🐳 이후 **Generate** 클릭<br>
🐳 파일이 하나 다운되는데 IntelliJ에서 열어보자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/basic/1-2.png"/>
</p>

🐳 처음 IntelliJ로 실행시 라이브러리 다운로드 시간이 약간 걸리는데 당황하지 말자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/basic/1-3.png"/>
</p>

🐳 한 번 실행을 해보자면...<br>
🐳 아무런 변화가 없는데 ...? 정상이다. 아직은 에러만 없으면 정상!<br>
🐳 마지막으로 하나만 더 변경하자 **Settings -> Build -> Gradle** 모두 IntelliJ로 바꾸자(속도 문제)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/basic/1-4.png"/>
</p>

---

## 순수 Java로만 웹페이지를 구현해 보자.

* [Get Code 🌍](https://github.com/EasyCoding-7/spring-basic-tutorial/tree/1-2)

```

* (I) - Interface

---------------------        --------------------
|                   |        |                  |
| (I) MemberService | ---->  | MemberSericeImpl |
|                   |        |                  |
---------------------        --------------------

```

```java
// join과 find를 지원
public interface MemberService {
    void join(Member member);
    Member findMember(Long memberId);
}
```

```

* (I) - Interface

------------------------        --------------------------
|                      |        |                        |
| (I) MemberRepository | ---->  | MemoryMemberRepository |
|                      |        |                        |
------------------------        --------------------------

```

```java
public interface MemberRepository {
    void save(Member member);
    Member findById(Long memberId);
}
```

🐳 **(TIPs)** 테스트 코드 작성하기<Br>
🐳 test 아래에 아래와 같이 클래스를 생성 후 아래의 코드를 넣자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/basic/1-5.png"/>
</p>

```java
public class MemberServiceTest {
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

🐳 조금 더 기능을 추가해 보자.

```

* (I) - Interface

----------------------        ---------------------
|                    |        |                   |
| (I) DiscountPolicy | ---->  | FixDiscountPolicy |
|                    |        |                   |
----------------------        ---------------------

```

```

* (I) - Interface

--------------------        --------------------
|                  |        |                  |
| (I) OrderService | ---->  | OrderServiceImpl |
|                  |        |                  |
--------------------        --------------------

```

🐳 새로운 할인 정책을 넣으려 한다.

```

* (I) - Interface

----------------------        ----------------------
|                    |        |                    |
| (I) DiscountPolicy | ---->  | RateDiscountPolicy |
|                    |        |                    |
----------------------        ----------------------

```

🐳 이제 됐나???<br>
🐳 유닛 테스트 추가`(Ctrl + Shift + t)`해 테스트 해보자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/basic/1-6.png"/>
</p>

```java
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

🐳 흠 ... 일단은 유닛 테스트까진 문제가 없어 보이는데...? 실제로 적용까지 해보자

```java
public class OrderServiceImpl implements OrderService {
    private final MemberRepository memberRepository = new
            MemoryMemberRepository();

    // 문제1) 할인 정잭을 변경하려고 OrderServiceImpl코드 자체를 만져야 한다??
    // private final DiscountPolicy discountPolicy = new FixDiscountPolicy();
    private final DiscountPolicy discountPolicy = new RateDiscountPolicy();

    // ...
```

🐳 문제를 해결해 보자.

```java
public class OrderServiceImpl implements OrderService {
    private DiscountPolicy discountPolicy;
    // ...
```

```java
// AppConfig에서 어떤 클래스를 선택할지 결정

public class AppConfig {
    public MemberService memberService() {
        return new MemberServiceImpl(new MemoryMemberRepository());
    }

    public OrderService orderService() {
        return new OrderServiceImpl(
                new MemoryMemberRepository(),
                new FixDiscountPolicy());
    }
}
```

{% endraw %}