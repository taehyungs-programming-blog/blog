---
layout: default
title: "03. Back-End가 동작하는 세 가지 방법 예시"
parent: "(입문)"
grand_parent: "Spring 🐍"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 웹페이지(Back-End)는 어떤식으로 구성이 될까?

* **정적 컨텐츠** - 일정한 화면이 있으며 서버는 그 화면을 그대로 클라이언트에게 보내준다(static)
* **MVC와 템플릿 엔진** - 큰 틀이 정해저 있으며 내부 컨텐츠를 서버에서 가공하여 보내준다(가장 많이 사용됨)
* **API** - JSON과 같은 포맷을 통하여 필요한 정보만 보내준다.

---

## 정적 컨텐츠 Example

🐍 `src\main\java\resources\static\static-example.html`에

```html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>static-example</title>
</head>
<body>
static-example
</body>
</html>
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/entry/entry-3-1.png"/>
</p>

---

* [Clone Code 🌍](https://github.com/EasyCoding-7/spring-entry/tree/3)

## MVC와 템플릿 엔진 Example

🐍 `src\main\java\Hello\hellospring\controller\HelloController.java`

```java
@Controller
public class HelloController {
    @GetMapping("hello")
    public String hello(Model model) {
        model.addAttribute("data", "hello!!");
        return "hello";
    }

    @GetMapping("hello-mvc")
    public String HelloMvc(@RequestParam("name") String name, Model model) {
        model.addAttribute("name", name);
        return "hello-template";
    }
}
```

🐍 `src\main\java\resources\template\hello-template.html`

```html
<html xmlns:th="http://www.thymeleaf.org">
<body>
<p th:text="'hello ' + ${name}">Heoll! empty</p>
</body>
</html>
```

🐍 `http://localhost:8080/hello-mvc?name=spring1234`로 접속

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/entry/entry-3-2.png"/>
</p>

---

* [Clone Code 🌍](https://github.com/EasyCoding-7/spring-entry/tree/3-2)

## API Example

🐍 `src\main\java\Hello\hellospring\controller\HelloController.java`

```java
@Controller
public class HelloController {
    // ...

    @GetMapping("hello-string")
    @ResponseBody   // Body에 return값을 그대로 넣어준다(아래 캡쳐를 보면 쉽게 이해 됨.)
    public String helloString(@RequestParam("name") String name) {
        return "hello " + name;
    }
}
```

🐍 `http://localhost:8080/hello-string?name=ttteeesssttt` 접속

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/entry/entry-3-3.png"/>
</p>

🐍 `<html>`과 같은 테그가 모두 사라졌음을 알 수 있다.

🐍 자 이제 활용해보자.!

```java
@Controller
public class HelloController {
    // ...

    @GetMapping("hello-api")
    @ResponseBody
    public Hello helloApi(@RequestParam("name") String name) {
        Hello hello = new Hello();
        hello.setName(name);
        return hello;
    }

    static class Hello{
        private String name;

        public String getName() {
            return name;
        }

        public void setName(String n) {
            name = n;
        }
    }
}
```

🐍 `http://localhost:8080/hello-api?name=ttteeesssttt` 접속

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/entry/entry-3-4.png"/>
</p>

🐍 자동 JSON형태로 변환해주네??

```java
@Controller
public class HelloController {
    // ...

@GetMapping("hello-api")
    @ResponseBody
    public Hello helloApi(@RequestParam("name") String name) {
        Hello hello = new Hello();
        hello.setName(name);
        hello.setInteger(10);
        return hello;
    }

    static class Hello{
        private String name;
        private String name2;
        private int n;

        public String getName() {
            return name;
        }

        public String getName2() {
            return name2;
        }

        public int getInteger() {
            return n;
        }

        public void setName(String n) {
            name = n;
            name2 = n;
        }

        public void setInteger(int i) {
            n = i;
        }
    }
}
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/entry/entry-3-5.png"/>
</p>

🐍 참고로 `getXXX()`가 있어야 JSON으로 만들어 준다