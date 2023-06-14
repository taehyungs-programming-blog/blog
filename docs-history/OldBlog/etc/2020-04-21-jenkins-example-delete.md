---
title: "Jenkins : delete file and folder"
permalink: gradle/example/delete/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-21 00:00:00 -0000
last_modified_at: 2020-04-21 00:00:00 -0000
sidebar:
  title: "etc"
  nav: etc
---

```groovy
task delete_gitignore_files(type: Delete) {
    // delete files
    delete fileTree(dir: "D:/temp/mygradle/test/").matching {
        include "**/*.gitignore"
    }

    // delete folder
    delete 'D:/temp/mygradle/test/bin'
    delete 'test/.gitignore'
}

// 숨겨진 파일을 삭제하기위해서 아래 옵션을 넣어야함(like .gitignore)
org.apache.tools.ant.DirectoryScanner.removeDefaultExclude("**/.gitignore")
```