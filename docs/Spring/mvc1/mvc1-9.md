---
layout: default
title: "09. log"
parent: "(MVC1)"
grand_parent: "Spring 🐍"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## War와 Jar

* JSP를 사용하지 않는다면 Jar를 사용하는 것이 좋습니다. 
* 앞으로 스프링 부트를 사용하면 Jar를 주로 사용하게 됩니다. 
* Jar를 사용하면 항상 **내장 서버**(톰캣등)를 사용하고, **webapp 경로도 사용하지 않습니다**. 
    * **내장 서버 사용에 최적화 되어 있는 기능**입니다. 최근에는 주로 이 방식을 사용합니다. 
* War를 사용하면 내장 서버도 사용가능 하지만, 주로 **외부 서버에 배포**하는 목적으로 사용합니다.

---

## log

```java
package hello.springmvc.basic;

import lombok.extern.slf4j.Slf4j;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

// @RestController로 return시 stirng이 그대로 return됨
// 참고) @Controller는 View가 리턴됨
@RestController
public class LogTestController {
    private final Logger log = LoggerFactory.getLogger(getClass());

    @RequestMapping("/log-test")
    public String logTest() {
        String name = "Spring";

        log.trace("trace log={}", name);
        log.debug("debug log={}", name);
        log.info(" info log={}", name);
        log.warn(" warn log={}", name);
        log.error("error log={}", name);
        //로그를 사용하지 않아도 a+b 계산 로직이 먼저 실행됨, 이런 방식으로 사용하면 X
        // 연산이 발생한다는게 핵심(쓸데없는 데이터 낭비)
        log.debug("String concat log=" + name);
        return "ok";
    }
}
```

* 하위 로그레벨 설정

```
# application.properties

#전체 로그 레벨 설정(기본 info)
logging.level.root=info

#hello.springmvc 패키지와 그 하위 로그 레벨 설정
logging.level.hello.springmvc=debug
```

```java
package hello.springmvc.basic;

import lombok.extern.slf4j.Slf4j;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

@Slf4j    // 를 넣으면 Logger 객체를 생성하지 않아도 된다
@RestController
public class LogTestController {

    @RequestMapping("/log-test")
    public String logTest() {
        String name = "Spring";

        log.trace("trace log={}", name);
        log.debug("debug log={}", name);
        log.info(" info log={}", name);
        log.warn(" warn log={}", name);
        log.error("error log={}", name);
        //로그를 사용하지 않아도 a+b 계산 로직이 먼저 실행됨, 이런 방식으로 사용하면 X
        // 연산이 발생한다는게 핵심(쓸데없는 데이터 낭비)
        log.debug("String concat log=" + name);
        return "ok";
    }
}
```