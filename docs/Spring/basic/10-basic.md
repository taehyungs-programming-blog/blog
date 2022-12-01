---
layout: default
title: "10. Web Scope"
parent: "(기초)"
grand_parent: "Spring 🐍"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 웹 스코프

* 웹 스코프의 특징 
    * 웹 스코프는 웹 환경에서만 동작한다. 
    * 웹 스코프는 프로토타입과 다르게 스프링이 해당 스코프의 종료시점까지 관리한다. 따라서 종료 메서드가 호출된다

* 웹 스코프 종류 
    * request: HTTP 요청 하나가 들어오고 나갈 때 까지 유지되는 스코프, 각각의 HTTP 요청마다 별도의 빈 인스턴스가 생성되고, 관리된다. 
    * session: HTTP Session과 동일한 생명주기를 가지는 스코프 
    * application: 서블릿 컨텍스트( ServletContext )와 동일한 생명주기를 가지는 스코프 
    * websocket: 웹 소켓과 동일한 생명주기를 가지는 스코프

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/basic/basic-10-1.png"/>
</p>

---

## request 스코프 예제 만들기

* build.gradle 수정

```gradle
dependencies {
   implementation 'org.springframework.boot:spring-boot-starter'


   //web 라이브러리 추가
   implementation 'org.springframework.boot:spring-boot-starter-web'
```

---

* 하고픈일 : 고객 UUID별로 Log를 별도로 남기게 해보자.

```java
@Component
@Scope(value = "request")
public class MyLogger {
    private String uuid;
    private String requestURL;

    public void setRequestURL(String requestURL) {
        this.requestURL = requestURL;
    }

    public void log(String message) {
        System.out.println("[" + uuid + "]" + "[" + requestURL + "] " +
                message);
    }

    @PostConstruct
    public void init() {
        uuid = UUID.randomUUID().toString();
        System.out.println("[" + uuid + "] request scope bean create:" + this);
    }
    
    @PreDestroy
    public void close() {
        System.out.println("[" + uuid + "] request scope bean close:" + this);
    }
}
```

```java
@Controller
@RequiredArgsConstructor
public class LogDemoController {
    private final LogDemoService logDemoService;
    private final MyLogger myLogger;

    @RequestMapping("log-demo")
    @ResponseBody
    public String logDemo(HttpServletRequest request) {
        String requestURL = request.getRequestURL().toString();
        myLogger.setRequestURL(requestURL);
        myLogger.log("controller test");
        logDemoService.logic("testId");
        return "OK";
    }
}
```

```java
@Service
@RequiredArgsConstructor
public class LogDemoService {
    private final MyLogger myLogger;

    public void logic(String id) {
        myLogger.log("service id = " + id);
    }
}
```

* 과연 원하는데로 출력이 될까??

```
Error creating bean with name 'myLogger': Scope 'request' is not active for the current thread; consider defining a scoped proxy for this bean if you intend to refer to it from a singleton;
```

* 스프링 애플리케이션을 실행 시키면 오류가 발생한다. 메시지 마지막에 싱글톤이라는 단어가 나오고… 스프링 애플리케이션을 실행하는 시점에 싱글톤 빈은 생성해서 주입이 가능하지만, request 스코프 빈은 아직 생성되지 않는다. 이 빈은 실제 고객의 요청이 와야 생성할 수 있다!

```java
@Controller
@RequiredArgsConstructor
public class LogDemoController {

    // 여기서 Autowired하게 되는데
    private final LogDemoService logDemoService;
    // (문제점) MyLogger는 request scope라 아직 bean이 없음.
    private final MyLogger myLogger;
```

---

## 해결법

```java
@Controller
@RequiredArgsConstructor
public class LogDemoController {
    private final LogDemoService logDemoService;
    // ObjectProvider를 쓰면 DI가 아니라 DL(Lookup)이 되어 찾게된다.
    private final ObjectProvider<MyLogger> myLoggerProvider;
    
    
    @RequestMapping("log-demo")
    @ResponseBody
    public String logDemo(HttpServletRequest request) {
        String requestURL = request.getRequestURL().toString();
        MyLogger myLogger = myLoggerProvider.getObject();
        myLogger.setRequestURL(requestURL);
        myLogger.log("controller test");
        logDemoService.logic("testId");
        return "OK";
    }
}
```

```java
@Service
@RequiredArgsConstructor
public class LogDemoService {
    private final ObjectProvider<MyLogger> myLoggerProvider;
    public void logic(String id) {
        MyLogger myLogger = myLoggerProvider.getObject();
        myLogger.log("service id = " + id);
    }
}
```

---

## 마지막 의문점

```java
private final ObjectProvider<MyLogger> myLoggerProvider;
// 이걸
    
private final MyLogger myLogger;
//이렇겐 못쓰나
```

### 스코프와 프록시

```java
@Component
@Scope(value = "request", proxyMode = ScopedProxyMode.TARGET_CLASS)
public class MyLogger {
    private String uuid;
```

```java
@Service
@RequiredArgsConstructor
public class LogDemoService {
    private final MyLogger myLogger;
    public void logic(String id) {
        MyLogger myLogger = myLogger.getObject();
        myLogger.log("service id = " + id);
    }
}
```

* CGLIB라는 라이브러리로 내 클래스를 상속 받은 가짜 프록시 객체를 만들어서 주입한다.