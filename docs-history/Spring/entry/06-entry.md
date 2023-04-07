---
layout: default
title: "06. MVC 기반 회원 관리 예제 만들기"
parent: "(입문)"
grand_parent: "Spring 🐍"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Clone Code 🌍](https://github.com/EasyCoding-7/spring-entry/tree/6-1)

```java
// HomeController.java

@Controller
public class HomeController {
    @GetMapping("/")
    public String home() {
        return "home";
    }
}
```

```html
<!-- home.html -->

<!DOCTYPE HTML>
<html xmlns:th="http://www.thymeleaf.org">
<body>
<div class="container">
    <div>
        <h1>Hello Spring</h1>
        <p>회원 기능</p>
        <p>
            <a href="/members/new">회원 가입</a>
            <a href="/members">회원 목록</a>
        </p>
    </div>
</div> <!-- /container -->
</body>
</html>
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/entry/entry-6-1.png"/>
</p>

```java
@Controller
public class MemberController {
    private final MemberService memberService;

    @Autowired
    public MemberController(MemberService memberService) {

        this.memberService = memberService;
    }

    @GetMapping(value = "/members/new")
    public String createForm() {
        return "members/createMemberForm";
    }
}
```

```html
<!DOCTYPE HTML>
<html xmlns:th="http://www.thymeleaf.org">
<body>
<div class="container">
  <form action="/members/new" method="post">
    <div class="form-group">
      <label for="name">이름</label>
      <input type="text" id="name" name="name" placeholder="이름을
입력하세요">
    </div>
    <button type="submit">등록</button>
  </form>
</div> <!-- /container -->
</body>
</html>
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/entry/entry-6-2.png"/>
</p>

```java
package hello.hellospring.domain;

public class MemberForm {
    private String name;

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }
}
```

```java
package hello.hellospring.controller;

import hello.hellospring.domain.Member;
import hello.hellospring.domain.MemberForm;
import hello.hellospring.service.MemberService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;

@Controller
public class MemberController {
    private final MemberService memberService;

    @Autowired
    public MemberController(MemberService memberService) {

        this.memberService = memberService;
    }

    @GetMapping(value = "/members/new")
    public String createForm() {
        return "members/createMemberForm";
    }

    @PostMapping(value ="/members/new")
    public String create(MemberForm form)
    {
        Member member = new Member();
        member.setName(form.getName());
        memberService.join(member);
        return "redirect:/";
    }
}
```

```java
@Controller
public class MemberController {
    private final MemberService memberService;

    // ...

    @GetMapping(value = "/members")
    public String list(Model model) {
        List<Member> members = memberService.findMembers();
        model.addAttribute("members", members);
        return "members/memberList";
    }
}
```

```html
<!DOCTYPE HTML>
<html xmlns:th="http://www.thymeleaf.org">
<body>
<div class="container">
  <div>
    <table>
      <thead>
<tr>
  <th>#</th>
  <th>이름</th>
</tr>
</thead>
<tbody>
<tr th:each="member : ${members}">
  <td th:text="${member.id}"></td>
  <td th:text="${member.name}"></td>
</tr>
</tbody>
</table>
</div>
</div> <!-- /container -->
</body>
</html>
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/entry/entry-6-3.png"/>
</p>