---
title: "Jenkins : .bat 파일 실행하기"
permalink: gradle/example/startbat/                # link 직접 지정
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
task exeTask{
    def result = exec {
        workingDir "D:/temp/mygradle"
        commandLine 'cmd', '/C', 'start.bat'
    }

    println result.toString()
}
```