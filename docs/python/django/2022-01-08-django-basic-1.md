---
layout: default
title: (Django) 1. Django는 왜 써야하나?
parent: (Django)
grand_parent: Python
nav_order: 5
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Django의 필요성?

예시로 필요성을 설명하겠다.

```html
<!-- index.html -->
<html>
	<body>
	<h1><a href="index.html">WEB</a></h1>
	<ol>
		<li><a href="1.html">HTML></a></li>
		<li><a href="2.html">HTML2</a></li>
        <!-- 
            ...
            1억개가 있다고 가정하자 
        -->
	</ol>
	</body>
</html>
```

```html
<!-- 1.html -->
<!-- 이런 파일이 1억개... -->
<html>
	<body>
	<h1><a href="index.html">WEB</a></h1>
	<ol>
		<li><a href="1.html">HTML></a></li>
		<li><a href="2.html">HTML2</a></li>
        <!-- 
            ...
            1억개가 있다고 가정하자 
        -->
	</ol>
	</body>
</html>
```

😺 만약 1억 1번째 페이지가 추가된다면, 1억개의 페이지를 모두 수정하며 `<li><a href="1000...html">HTML></a></li>`를 추가해야 할까?? (👉 세상에 이런 비효율이??😿)

😺 이런 반복되는 작업을 python이 대신해준다면? 👉 그 대신 해주는 녀석의 이름이 **django**라면?!?!

<br>

😸 **결론** - **django**는 간단히 말해서 사용자의 요청이 올 때마다 python을 이용해 웹페이지를 제작하여 응답(Response)해주는 web framework 중 하나이다.<br>
(이렇게 설명하면 이해가 되지 않으니 자세한건 뒤로 넘어가면서 차차배우자)

<br>

> [참고한 사이트 (유튜부) 🌎](https://www.youtube.com/watch?v=pbKhn2ten9I)