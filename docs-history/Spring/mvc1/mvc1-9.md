---
layout: default
title: "09. Spring MVC 기본 기능 - logging"
parent: "(MVC1)"
grand_parent: "Spring 🐍"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## (TIPS) War와 Jar

* JSP를 사용하지 않는다면 Jar를 사용하는 것이 좋습니다. 
* 앞으로 스프링 부트를 사용하면 Jar를 주로 사용하게 됩니다. 
* Jar를 사용하면 항상 **내장 서버**(톰캣등)를 사용하고, **webapp 경로도 사용하지 않습니다**. 
    * **내장 서버 사용에 최적화 되어 있는 기능**입니다. 최근에는 주로 이 방식을 사용합니다. 
* War를 사용하면 내장 서버도 사용가능 하지만, 주로 **외부 서버에 배포**하는 목적으로 사용합니다.

---

## log

* 참고) slf4j는 log의 interface고 실제로 사용되는 log는 logback이다.

```java
//@Slf4j
@RestController    // 반환을 view로 하는것이 아니라 http body로 반환(일단은 그냥 받아들이자.)
// Cf) @Controller는 String반환시 View를 찾게된다.
public class LogTestController {

    private final Logger log = LoggerFactory.getLogger(getClass());

    @RequestMapping("/log-test")
    public String logTest() {
        String name = "Spring";

        // System.out으로는 이제 로그를 남기지 말자.
        System.out.println("name = " + name);

        // 그리 좋은 방식은 아니다
        log.trace(" trace my log="+ name);

        // 이런방식으로 로그를 남기자.
            // why? - + 연산을 할경우 trace함수에 들어가기전 문자열을 합치는 연산을 java스스로 먼저하게된다.
            // trace를 호출할지 안할지 모르는데 연산을 먼저해버린다.??? -> 자원의 손실이다.
        log.trace("trace log={}", name);

        log.debug("debug log={}", name);
        log.info(" info log={}", name);
        log.warn(" warn log={}", name);
        log.error("error log={}", name);

        return "ok";
    }
}
```

* application.properties

```
# 로그레벨 설정하기
logging.level.root=info

logging.level.hello.springmvc=debug
```

* LEVEL: TRACE > DEBUG > INFO(default) > WARN > ERROR

```java
@Slf4j
@RestController
public class LogTestController {

    // Slf4j에 log가 자동포함되어 아래가 필요없음.
    // private final Logger log = LoggerFactory.getLogger(getClass());

    @RequestMapping("/log-test")
    public String logTest() {

        log.trace(" trace my log="+ name);

        log.trace("trace log={}", name);

        log.debug("debug log={}", name);
        log.info(" info log={}", name);
        log.warn(" warn log={}", name);
        log.error("error log={}", name);

        return "ok";
    }
}
```