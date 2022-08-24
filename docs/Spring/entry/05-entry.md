---
layout: default
title: "05. 컴포넌트 스캔과 자동 의존관계 설정"
parent: "(입문)"
grand_parent: "Spring 🐍"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Clone Code 🌍](https://github.com/EasyCoding-7/spring-entry/tree/5-1)

## 컴포넌트 스캔과 자동 의존관계 설정

🐍 설명 전, Spring의 **[ Controller ] - [ Service ] - [ Repository ]**의 관계를 알아야 한다.<br>

## Controller

🐍 **Controller** - http로 접속시 어느 html로 연결할지 안내한다.

```java
// 예를 들어보자면

@Controller
public class HelloController {

    @GetMapping("hello")
    public String hello(Model model) {
        model.addAttribute("data", "hello!!");
        return "hello";            // hello.html로 연결
    }

    @GetMapping("hello-mvc")
    public String HelloMvc(@RequestParam("name") String name, Model model) {
        model.addAttribute("name", name);
        return "hello-template";    // hello-template.html로 연결
    }
```

🐍 **[ Controller ] - (html(View)) - [ Service ] - [ Repository ]** 대략 이런 구조이다.<br>
🐍 추가적으로 View(html)에서의 요청을 Service가 받고, Repository에서 데이터를 읽어오는 구조이다.

---

🐍 오늘 배울 내용은 Spring에서 **Service**와 **Repository**를 어떻게 만드냐 이다.<br>
🐍 우선 들어가기 전 기존 controller에 약간 수정이 필요한데

```java
@Controller
public class MemberController {

    private final MemberService memberService;

    @Autowired
    public MemberController(MemberService memberService) {
        this.memberService = memberService;
    }
}
```

🐍 기존과 차이점은 `Autowired`를 이용해 **Spring Bean(object)**를 연결한다는 것이다. (왜 이렇게 하는지는 이후에 설명, 큰 장점이 있다)

```java
@Service
public class MemberService {
    private final MemberRepository memberRepository;
    @Autowired
    public MemberService(MemberRepository memberRepository) {
        this.memberRepository = memberRepository;
    }
}
```

```java
@Repository
public class MemoryMemberRepository implements MemberRepository {}
```

🐍 핵심은 **Controller, Service, Repositry**를 `@`를 이용해 Spring에 등록한다는 것!

```java
// ... (참고)

// 실행과 동시에 ComponentScan을 돈다
@ComponentScan(excludeFilters = { @Filter(type = FilterType.CUSTOM, classes = TypeExcludeFilter.class),
      @Filter(type = FilterType.CUSTOM, classes = AutoConfigurationExcludeFilter.class) })
public @interface SpringBootApplication {
```

---

## 자바 코드로 직접 스프링 빈 생성

```java
@Configuration
public class SpringConfig {
    @Bean
    public MemberService memberService() {
    return new MemberService(memberRepository());
 }

    @Bean
    public MemberRepository memberRepository() {
    return new MemoryMemberRepository();
 }
}
```