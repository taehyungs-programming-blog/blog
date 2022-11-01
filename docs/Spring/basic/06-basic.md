---
layout: default
title: "06. Bean이 아닌 일반 Java Object의 의존주입? + lombok"
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
// Member는 bean이 아니라 java object이다.

// 호출자체가 안됨 -> required = false
@Autowired(required = false)
public void setNoBean1(Member member) {
    System.out.println("setNoBean1 = " + member);
}

//null 호출
@Autowired
public void setNoBean2(@Nullable Member member) {
    System.out.println("setNoBean2 = " + member);
}

//Optional.empty 호출
@Autowired(required = false)
public void setNoBean3(Optional<Member> member) {
    System.out.println("setNoBean3 = " + member);
}
```

---

## lombok

```java
@Component
public class OrderServiceImpl implements OrderService 
{
    private final MemberRepository memberRepository;
    private final DiscountPolicy discountPolicy;

    // 사실 아래의 생성자 의존성 주입은 매번하는건데..
    // 좀 더 편하게 못하나?
    @Autowired
    public OrderServiceImpl(MemberRepository memberRepository, DiscountPolicy discountPolicy) {
        this.memberRepository = memberRepository;
        this.discountPolicy = discountPolicy;
    }

    // ...
```

```java
@Component
@RequiredArgsConstructor
public class OrderServiceImpl implements OrderService 
{
    // 대략 이렇게 하고싶다는 말.
    private final MemberRepository memberRepository;
    private final DiscountPolicy discountPolicy;

    // ...
```

* [Clone Code 🌎](https://github.com/EasyCoding-7/spring_basic/tree/18)

* build.gradle 수정
* 오른쪽 위의 코끼리 fresh버튼을 눌러서 다시 설치한다.

```gradle
plugins {
	id 'org.springframework.boot' version '2.7.5'
	id 'io.spring.dependency-management' version '1.0.15.RELEASE'
	id 'java'
}

group = 'hello'
version = '0.0.1-SNAPSHOT'
sourceCompatibility = '17'

//lombok 설정 추가 시작
configurations {
	compileOnly {
		extendsFrom annotationProcessor
	}
}
//lombok 설정 추가 끝

repositories {
	mavenCentral()
}

dependencies {
	implementation 'org.springframework.boot:spring-boot-starter'
	//lombok 라이브러리 추가 시작
	compileOnly 'org.projectlombok:lombok'
	annotationProcessor 'org.projectlombok:lombok'
	testCompileOnly 'org.projectlombok:lombok'
	testAnnotationProcessor 'org.projectlombok:lombok'
	//lombok 라이브러리 추가 끝
	testImplementation 'org.springframework.boot:spring-boot-starter-test'
}

tasks.named('test') {
	useJUnitPlatform()
}
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/basic/basic-6-1.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/basic/basic-6-2.png"/>
</p>