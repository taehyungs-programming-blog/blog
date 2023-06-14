---
title: "Gradle : 의존관계"
permalink: gradle/dependencies/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-19 00:00:00 -0000
last_modified_at: 2020-04-19 00:00:00 -0000
sidebar:
  title: "etc"
  nav: etc
---

* [강좌](https://www.youtube.com/watch?v=hAtN9pd9xGg&list=PL7mmuO705dG2pdxCYCCJeAgOeuQN1seZz&index=21)

```groovy
// build를 위한 환경 구성/환경 설정
configurations{
    conf1
}

// 의존관계 정의
dependencies{
    conf1 gradleApi()       // 의존관계 지정
}

task exeTask{
    doLast{
        configurations.conf1.each{
            println it.absolutePath
        }
    }
}
```

![](/file/image/gradle-dependencies01.png)

```groovy
// build를 위한 환경 구성/환경 설정
configurations{
    conf1
}

// 저장소 정의
repositories{
    mavenCentral()
}

// 의존관계 정의
dependencies{
    // 그룹:모듈명:버전
    conf1 'ch.qos.logback:logback-classic:1.0.13'
    conf1 'org.springframework:spring-orm:4.0.2.RELEASE'
    conf1 (group: 'org.gradle.api.plugins', name:'gradle-cargo-plugin',version:'0.6.1')
}

task exeTask{
    doLast{
        configurations.conf1.each{
            println it.absolutePath
        }
    }
}
```

![](/file/image/gradle-dependencies02.png)

---

```groovy
configurations{
    conf1
}

// 의존관계 정의
dependencies{
    // 파일 의존관계 - 특정 파일이나 라이브러리 지정
    conf1 files("lib/commonLib.jar")
    // 파일 의존관계 - 디렉터리 지정
    conf1 fileTree(dir:"lib", include:"**/*.jar")
}

task exeTask{
    doLast{
        configurations.conf1.each{
            println it.absolutePath
        }
    }
}
```

![](/file/image/gradle-dependencies03.png)



