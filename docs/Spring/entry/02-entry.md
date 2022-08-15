---
layout: default
title: "02. 기본 페이지 띄우기"
parent: "(입문)"
grand_parent: "Spring 🐍"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Clone Code 🌍](https://github.com/EasyCoding-7/spring-entry/tree/2)

## 정적 페이지 넣기

🐍 `src\main\java\resources\static\index.html`에

```html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Hello</title>
</head>
<body>
Hello
</body>
</html>
```

🐍 `localhost:8080`에 접속하면 Hello가 출력된다.<br>
🐍 `index.html`은 `localhost:8080`라는 것을 알 수 있다.

---

## 동작 페이지 넣기

🐍 TimeLeaf사용해 동적페이지 만들기<br>
🐍 `src\main\java\Hello\hellospring\controller\HelloController.java`

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/entry/entry-2-1.png"/>
</p>

```java
@Controller
public class HelloController {
    @GetMapping("hello")
    public String hello(Model model) {
        model.addAttribute("data", "hello!!");
        return "hello";
    }
}
```

🐍 `src\main\java\resources\template\hello.html`

```html
<!DOCTYPE HTML>
<html xmlns:th="http://www.thymeleaf.org">
<head>
  <title>Hello</title>
  <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
</head>
<body>
<p th:text="'안녕하세요. ' + ${data}" >안녕하세요. 손님</p>
</body>
</html>
```

🐍 아직은 뭘 하는지 잘 모르겠으나 일단은 받아들이고 넘어가자.<Br>
🐍 **Q.** `안녕하세요. 손님`은 왜 출력이 안되나요? - 서버가 돌면 `<p th:text="'안녕하세요. ' + ${data}" >`의 내용으로 `안녕하세요. 손님`이 치환된다. 그럼 `안녕하세요. 손님`를 왜 적었냐? 순수 html코드로 보면 아무런 내용이 출력되지 않으며 Front-End구현에 어려움이 있기 때문이다.

---

## 빌드 후 실행

```sh
$ gradlew.bat build
```

```sh
$ cd ./build/libs
$ java -jar hello-spring-0.0.1-SNAPSHOT.jar
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/entry/entry-2-2.png"/>
</p>

🐍 너무 편하다 ㅎㅎ;;