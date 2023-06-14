---
title: "page sample"
permalink: sample/page-sample/ # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-10 00:00:00 -0000
last_modified_at: 2020-03-25 00:00:00 -0000
sidebar:
  title: "Sample Title"
  nav: sidebar-sample
---

될 수 있은면 이 첫 줄에 페이지 내용을 간결하게 정리하여 적는게 좋다.

## sub title 1

page 123

## sub title 2

page 123 123 12

## link 

* [link](/gitpage-adsense/)

```
[link](/gitpage-adsense/)
```

## edit side bar navigation

> * [참고사이트](https://mmistakes.github.io/minimal-mistakes/layout-sidebar-nav-list/)

```yml
# 포스트 글 상단에 sidebar: 옵션을 넣는다
---
title: "page sample"
permalink: sample/page-sample/ # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-10 00:00:00 -0000
last_modified_at: 2020-03-25 00:00:00 -0000
sidebar:
  title: "Sample Title"
  nav: sidebar-sample
---
```

```yml
# \data\navigation.yml link
# for sidebar

# sidebar navigation list sample
sidebar-sample:
  - title: "Parent Page A"
    children:
      - title: "Child Page A1"
        url: /
```