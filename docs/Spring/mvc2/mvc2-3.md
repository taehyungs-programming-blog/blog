---
layout: default
title: "03. 국제화"
parent: "(MVC2)"
grand_parent: "Spring 🐍"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Clone code 🌎](https://github.com/EasyCoding-7/spring-mvc2-example/tree/main/message)

---

## IntelliJ에서 언어가 깨진다면?

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/mvc2/mvc2-3-1.png"/>
</p>

---

## 언어파일 설정

```
# messages.properties

hello=안녕
hello.name=안녕 {0}

label.item=상품
label.item.id=상품 ID
label.item.itemName=상품명
label.item.price=가격
label.item.quantity=수량

page.items=상품 목록
page.item=상품 상세
page.addItem=상품 등록
page.updateItem=상품 수정

button.save=저장
button.cancel=취소
```

```
# messages_en.properties

hello=hello
hello.name=hello {0}

label.item=Item
label.item.id=Item ID
label.item.itemName=Item Name
label.item.price=price
label.item.quantity=quantity

page.items=Item List
page.item=Item Detail
page.addItem=Item Add
page.updateItem=Item Update

button.save=Save
button.cancel=Cancel
```

```
# application.properties
# messages가 기본 파일이라고 알린다.

spring.messages.basename=messages
```

```html
<div class="container">

<div class="py-5 text-center">
    <h2 th:text="#{page.addItem}">상품 등록</h2>
</div>
```

* 결론
    * 크롬 브라우저 설정 언어를 검색하고, 우선 순위를 변경하면 된다.
    * 우선순위를 영어로 변경하고 테스트해보자.
    * 웹 브라우저의 언어 설정 값을 변경하면 요청시 Accept-Language 의 값이 변경된다.