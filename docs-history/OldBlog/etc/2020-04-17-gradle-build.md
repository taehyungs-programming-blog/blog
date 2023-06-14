---
title: "Gradle : build"
permalink: gradle/build/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-17 00:00:00 -0000
last_modified_at: 2020-04-17 00:00:00 -0000
sidebar:
  title: "etc"
  nav: etc
---

## gradle script

* init script(gradle 객체)
* setting script(setting 객체)
* build script(project 객체)

---

* 그루비 문법구조 이해하기
    * http://groovy-lang.org/
    * java 플랫폼 기반 문법 언어

```groovy
def id = 'gradle'       // 변수를 지정가능

// 물론 형식을 지정할 수 있다.
string id = 'gradle'
```

```groovy
// 문자열 입력
def id = ```gradle
        Groovy
        script```

def id = """gradle
        Groovy
        script"""
```

```groovy
// list and map
def id = ['grade', 'Groovy']
id[1] = 'script'

def id = ['a':'grade', 'b':'Groovy']
assert id['a'] == 'gradle'
```

---

* gradle script 파일 구조

```groovy
// 처리문
def id = 'gradle'

// 스크립트 블록
repositories{       // 저장소 설정(스크립트 블록은 검색하면 나옴.)
    mavenCentral()
}

task idCheck<<{
    def id = 'check'
    println 'id : ' + id
}
```

* [gradle script block](https://docs.gradle.org/current/userguide/groovy_build_script_primer.html#groovy:blocks)

```groovy
// 시스템 속성
task hello<<{
    println System.properties['message']
}

// 확장 속성
ext{
    extPro1 = 'pro1'
    extPro2 = 'pro2'
}

println '속성값1 : ' +ext.extPro1
println '속성값2 : ' +ext.extPro2
```

---

## gradle 간단 빌드 해보기

```groovy
// gradle.properties
msg=Hi, Gradle!
```

```groovy
// build.gradle
task hello {
    doLast{
        println msg
    }
}
```

```s
$ gradle hello
```

![](/file/image/gradle-build-01.png)
