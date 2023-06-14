---
title: "(gitpage) jekyll로 github page만들기"
date: 2020-03-09 00:00:00 -0000
---

> [minimal-mistakes](https://github.com/mmistakes/minimal-mistakes)를 기반으로 github page생성
> 우선 소스를 Fork

![](/file/image/gitpage-make-image-01.png)

> 로컬에 Clone

![](/file/image/gitpage-make-image-02.png)

> _config.yml을 다음과 같이 수정한다.<br>
> 아래 수정한 부분만 수정해주면 된다.

```yml
remote_theme           : "mmistakes/minimal-mistakes"
minimal_mistakes_skin    : "dark" # "air", "aqua", "contrast", "dark", "dirt", "neon", "mint", "plum", "sunrise"

# Site Settings
locale                   : "en-US"
title                    : "김태형의 매일 코딩"
title_separator          : "-"
subtitle                 : # site tagline that appears below site title in masthead
name                     : "김태형"
description              : "김태형의 매일 코딩 홈페이지"
url                      : "https://8bitscoding.github.io/"
```

> _posts 작성

![](/file/image/gitpage-make-image-03.png)

---

## 추가 수정

> _data\navigation.yml 내부 수정

```yml
# main links
main:
  - title: "HOME"
    url: https://8bitscoding.github.io/
  - title: "Electron"
    url: /categories-Electron/
  - title: "Qt"
    url: /categories-Qt/
  - title: "C++"
    url: /categories-C++/
  - title: "JavaScript"
    url: /categories-JS/
  - title: "etc"
    url: /categories-etc/
```

> _config.yml은 수정부분이 많기에 그냥 깃을 참조

---

> * [참고사이트](https://xho95.github.io/blog/github/pages/jekyll/minima/theme/2017/03/04/Jekyll-Blog-with-Minima.html)