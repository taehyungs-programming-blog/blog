---
title: "git : git ignore 생성"
permalink: git/ignore/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-08-25 00:00:00 -0000
last_modified_at: 2020-08-25 00:00:00 -0000
sidebar:
  title: "etc"
  nav: etc
excerpt: ""
tag:
  - git
category:
  - ignoreh
header:
  teaser: /file/image/etc-page-teaser.gif
---

* [참고사이트](https://nesoy.github.io/articles/2017-01/Git-Ignore)

`.gitignore` 파일 생성

```
# : comments

# no .a files
*.a

# but do track lib.a, even though you're ignoring .a files above
!lib.a

# only ignore the TODO file in the current directory, not subdir/TODO
/TODO

# ignore all files in the build/ directory
build/

# ignore doc/notes.txt, but not doc/server/arch.txt
doc/*.txt

# ignore all .pdf files in the doc/ directory
doc/**/*.pdf
```