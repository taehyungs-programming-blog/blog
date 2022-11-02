---
layout: default
title: "01. 환경설정 + servlet사용해보기"
parent: "(MVC1)"
grand_parent: "Spring 🐍"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 환경설정

* [Clone Code 🌎](https://github.com/EasyCoding-7/spring-mvc1/tree/1)

* https://start.spring.io/ 에서 아래와 같이 설정

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/mvc1/mvc1-1-1.png"/>
</p>

* 늘상하는 설정 진행

* 무료버전일 경우 IntelliJ말고 Gradle을 설정해야한다고 한다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/mvc1/mvc1-1-2.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/mvc1/mvc1-1-3.png"/>
</p>

---

## Servlet 등록

* [Clone Code 🌎](https://github.com/EasyCoding-7/spring-mvc1/tree/2)

```java
package hello.servlet;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.boot.web.servlet.ServletComponentScan;

@ServletComponentScan //서블릿 자동
@SpringBootApplication
public class ServletApplication {

	public static void main(String[] args) {
		SpringApplication.run(ServletApplication.class, args);
	}

}
```

```java
package hello.servlet.basic;

import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.IOException;

@WebServlet(name = "helloServlet", urlPatterns = "/hello")
public class HelloServlet extends HttpServlet {

    @Override
    protected void service(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        // http://localhost:8080
        System.out.println("HelloServlet.service");
        System.out.println("request = " + request);
        System.out.println("response = " + response);

        // http://localhost:8080/hello?username=100
        String username = request.getParameter("username");
        System.out.println("username = " + username);

        response.setContentType("text/plain");
        response.setCharacterEncoding("utf-8");
        response.getWriter().write("hello " + username);
    }
}
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/mvc1/mvc1-1-4.png"/>
</p>

* **TIPS)** 요청 메시지 log로 확인하기

```
# application.properties내에 아래를 넣자

logging.level.org.apache.coyote.http11=debug
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/mvc1/mvc1-1-5.png"/>
</p>