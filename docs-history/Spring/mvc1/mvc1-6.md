---
layout: default
title: "06. Spring MVC 필요성?"
parent: "(MVC1)"
grand_parent: "Spring 🐍"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Clone Code 🌎](https://github.com/EasyCoding-7/spring-mvc1/tree/8)

* 서블릿과 JSP의 한계 서블릿으로 개발할 때는 뷰(View)화면을 위한 **HTML을 만드는 작업이 자바 코드에 섞여서 지저분하고 복잡**했다. 

```java
// Example
@WebServlet(name = "memberFormServlet", urlPatterns = "/servlet/members/newform") 
public class MemberFormServlet extends HttpServlet { 
    @Override 
    protected void service(HttpServletRequest request, HttpServletResponse response) 
            throws ServletException, IOException { 
        response.setContentType("text/html"); 
        response.setCharacterEncoding("utf-8"); 
        PrintWriter w = response.getWriter(); 
        w.write("<!DOCTYPE html>\n" + 
                "<html>\n" + 
                "<head>\n" + 
                " <meta charset=\"UTF-8\">\n" + 
                " <title>Title</title>\n" + 
                "</head>\n" + 
                "<body>\n" + 
                "<form action=\"/servlet/members/save\" method=\"post\">\n" + 
                " username: <input type=\"text\" name=\"username\" />\n" + 
                " age: <input type=\"text\" name=\"age\" />\n" + 
                " <button type=\"submit\">전송</button>\n" + 
                "</form>\n" + 
                "</body>\n" + 
                "</html>\n"); 
    } 
}
```

* JSP를 사용한 덕분에 뷰를 생성하는 HTML 작업을 깔끔하게 가져가고, 중간중간 동적으로 변경이 **필요한 부분에만 자바 코드**를 적용했다. 
* 그런데 이렇게 해도 해결되지 않는 몇가지 고민이 남는다. 회원 저장 JSP를 보자. 코드의 상위 절반은 회원을 저장하기 위한 비즈니스 로직이고, 나머지 하위 절반만 결과를 HTML로 보여주기 위한 뷰 영역이다. 

```jsp
<!-- webapp/jsp/members/save.jsp --> 
<%@ page import="hello.servlet.domain.member.MemberRepository" %> 
<%@ page import="hello.servlet.domain.member.Member" %> 
<%@ page contentType="text/html;charset=UTF-8" language="java" %> 

<!-- 로직부 -->
<% 
    // request, response 사용 가능 
    MemberRepository memberRepository = MemberRepository.getInstance(); 
    System.out.println("save.jsp"); 
    String username = request.getParameter("username"); 
    int age = Integer.parseInt(request.getParameter("age")); 
    Member member = new Member(username, age); 
    System.out.println("member = " + member); 
    memberRepository.save(member); 
%>

<!-- HTML 부 -->
<html> 
<head> 
    <meta charset="UTF-8"> 
</head> 
    <body> 
    성공 
    <ul> 
        <li>id=<%=member.getId()%></li> 
        <li>username=<%=member.getUsername()%></li> 
        <li>age=<%=member.getAge()%></li> 
    </ul> 
    <a href="/index.html">메인</a> 
</body> 
</html>
```

* 회원 목록의 경우에도 마찬가지다. 코드를 잘 보면, JAVA 코드, 데이터를 조회하는 리포지토리 등등 다양한 코드가 모두 JSP에 노출되어 있다. 
* **JSP가 너무 많은 역할을 한다.** 이렇게 작은 프로젝트도 벌써 머리가 아파오는데, 수백 수천줄이 넘어가는 JSP를 떠올려보면 정말 지옥과 같을 것이다. 

* **대충 View와 로직을 나누고 싶다는 말.**

* MVC 패턴의 등장 비즈니스 로직은 서블릿 처럼 다른곳에서 처리하고, JSP는 목적에 맞게 HTML로 화면(View)을 그리는 일에 집중하도록 하자. 
* 과거 개발자들도 모두 비슷한 고민이 있었고, 그래서 MVC 패턴이 등장했다. 우리도 직접 MVC 패턴을 적용해서 프로젝트를 리팩터링 해보자. 

---

## MVC 패턴

* 개요 너무 많은 역할 하나의 서블릿이나 JSP만으로 비즈니스 로직과 뷰 렌더링까지 모두 처리하게 되면, 너무 많은 역할을 하게되고, 결과적으로 유지보수가 어려워진다. 
* 비즈니스 로직을 호출하는 부분에 변경이 발생해도 해당 코드를 손대야 하고, UI를 변경할 일이 있어도 비즈니스 로직이 함께 있는 해당 파일을 수정해야 한다. 
* HTML 코드 하나 수정해야 하는데, 수백줄의 자바 코드가 함께 있다고 상상해보라! 또는 비즈니스 로직을 하나 수정해야 하는데 수백 수천줄의 HTML 코드가 함께 있다고 상상해보라. 
* 변경의 라이프 사이클 사실 이게 정말 중요한데, 진짜 문제는 둘 사이에 변경의 라이프 사이클이 다르다는 점이다. 

* 웹 애플리케이션은 보통 이 MVC 패턴을 사용한다. 
    * **컨트롤러**: HTTP 요청을 받아서 파라미터를 검증하고, 비즈니스 로직을 실행한다. 그리고 뷰에 전달할 결과 데이터를 조회해서 모델에 담는다. 
    * **모델**: 뷰에 출력할 데이터를 담아둔다. 뷰가 필요한 데이터를 모두 모델에 담아서 전달해주는 덕분에 뷰는 비즈니스 로직이나 데이터 접근을 몰라도 되고, 화면을 렌더링 하는 일에 집중할 수 있다. 
    * **뷰**: 모델에 담겨있는 데이터를 사용해서 화면을 그리는 일에 집중한다. 여기서는 HTML을 생성하는 부분을 말한다. > 참고 > 컨트롤러에 비즈니스 로직을 둘 수도 있지만, 이렇게 되면 컨트롤러가 너무 많은 역할을 담당한다. 그래서 일반적으로 비즈니스 로직은 서비스(Service)라는 계층을 별도로 만들어서 처리한다. 
* 그리고 컨트롤러는 비즈니스 로직이 있는 서비스를 호출하는 역할을 담당한다. 참고로 비즈니스 로직을 변경하면 비즈니스 로직을 호출하는 컨트롤러의 코드도 변경될 수 있다. 앞에서는 이해를 돕기 위해 비즈니스 로직을 호출한다는 표현 보다는, 비즈니스 로직이라 설명했다.

---

```java
package hello.servlet.web.servletmvc;

import javax.servlet.RequestDispatcher;
import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.IOException;

@WebServlet(name = "mvcMemberFormServlet", urlPatterns = "/servlet-mvc/members/new-form")
public class MvcMemberFormServlet extends HttpServlet {

    @Override
    protected void service(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {
        String viewPath = "/WEB-INF/views/new-form.jsp";
        RequestDispatcher dispatcher = request.getRequestDispatcher(viewPath);
        dispatcher.forward(request, response);
    }
}
```

```jsp
<!-- main/webapp/WEB-INF/views/members.jsp -->

<%@ page contentType="text/html;charset=UTF-8" language="java" %>
<%@ taglib prefix="c" uri="http://java.sun.com/jsp/jstl/core"%>
<html>
<head>
    <meta charset="UTF-8">
    <title>Title</title>
</head>
<body>
<a href="/index.html">메인</a>
<table>
    <thead>
    <th>id</th>
    <th>username</th>
    <th>age</th>
    </thead>
    <tbody>
    <c:forEach var="item" items="${members}">
        <tr>
            <td>${item.id}</td>
            <td>${item.username}</td>
            <td>${item.age}</td>
        </tr>
    </c:forEach>
    </tbody>
</table>
</body>
</html>
```

```java
package hello.servlet.web.servletmvc;

import hello.servlet.domain.member.Member;
import hello.servlet.domain.member.MemberRepository;

import javax.servlet.RequestDispatcher;
import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.IOException;
import java.util.List;

@WebServlet(name = "mvcMemberListServlet", urlPatterns = "/servlet-mvc/members")
public class MvcMemberListServlet extends HttpServlet {

    private MemberRepository memberRepository = MemberRepository.getInstance();

    @Override
    protected void service(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {
        System.out.println("MvcMemberListServlet.service");
        List<Member> members = memberRepository.findAll();
        request.setAttribute("members", members);
        String viewPath = "/WEB-INF/views/members.jsp";
        RequestDispatcher dispatcher = request.getRequestDispatcher(viewPath);
        dispatcher.forward(request, response);
    }
}
```

```jsp
<!-- main/webapp/WEB-INF/views/new-form.jsp -->

<%@ page contentType="text/html;charset=UTF-8" language="java" %>
<html>
<head>
    <meta charset="UTF-8">
    <title>Title</title>
</head>
<body>
<!-- 상대경로 사용, [현재 URL이 속한 계층 경로 + /save] -->
<form action="save" method="post">
    username: <input type="text" name="username" />
    age: <input type="text" name="age" />
    <button type="submit">전송</button>
</form>
</body>
</html>
```

```jsp
<!-- main/webapp/WEB-INF/views/save-result.jsp -->

<%@ page contentType="text/html;charset=UTF-8" language="java" %>
<html>
<head>
    <meta charset="UTF-8">
</head>
<body>
성공
<ul>
    <li>id=${member.id}</li>
    <li>username=${member.username}</li>
    <li>age=${member.age}</li>
</ul>
<a href="/index.html">메인</a>
</body>
</html>
```