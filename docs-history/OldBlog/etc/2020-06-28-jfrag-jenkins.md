---
title: "jFrag artifactory : jenkins 연동"
permalink: jFrag/jenkins/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-06-28 00:00:00 -0000
last_modified_at: 2020-06-28 00:00:00 -0000
sidebar:
  title: "etc"
  nav: etc
excerpt: ""
tag:
  - jFrag
category:
  - jenkins
header:
  teaser: /file/image/etc-page-teaser.gif
---

* [참고(Maven)](https://www.youtube.com/watch?v=AlaRlwCekY0)
* [참고(Gradle)]()

---

## Jenkins Plugin 설치

* Jenkins 관리 -> 플러그인 관리 -> Artifactory Plugin 설치

![](/file/image/jFrag-jenkins-01.png)

---

## item 생성

![](/file/image/jFrag-jenkins-02.png)

---

## Jenkins Artifactory Setting

* Jenkins 관리 -> 시스템 설정 -> JFrog 

![](/file/image/jFrag-jenkins-03.png)

![](/file/image/jFrag-jenkins-04.png)

* URL : http://localhost:8082/artifactory

---

## jFrog Gradle Repo 만들기

![](/file/image/jFrag-jenkins-05.png)

![](/file/image/jFrag-jenkins-06.png)

Jenkins에서 jFrog 검색시 나온다.

![](/file/image/jFrag-jenkins-07.png)

여기까지하고 저장하면 item이 다음과 같이 변하게 됨

![](/file/image/jFrag-jenkins-08.png)

---

## Gradle build Setting

![](/file/image/jFrag-jenkins-09.png)

---

## build.gradle 작성

* [Github](https://github.com/jfrog/project-examples/tree/master/gradle-examples)

```s
# 빌드는 이렇게...
$ gradle artifactoryPublish
```

```groovy
// build.gradle
/*
 * Copyright (C) 2013 JFrog Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

buildscript {
    repositories {
        jcenter()
    }
    dependencies {
        classpath(group: 'org.jfrog.buildinfo', name: 'build-info-extractor-gradle', version: '4.+')
    }
}

allprojects {
    repositories {
        maven {
            url "http://127.0.0.1:8081/artifactory/jcenter"
        }
    }
}

version = currentVersion

apply plugin: 'com.jfrog.artifactory'
apply plugin: 'java'
apply plugin: 'maven-publish'

dependencies {
    testImplementation 'junit:junit:4.7'
}

publishing {
    publications {
        mavenJava(MavenPublication) {
            from components.java
        }
    }
}

artifactory {
    contextUrl = 'http://127.0.0.1:8081/artifactory'
    publish {
        repository {
            repoKey = 'libs-snapshot-local' // The Artifactory repository key to publish to
            username = "${artifactory_user}" // The publisher user name
            password = "${artifactory_password}" // The publisher password
        }
        defaults {
            // Reference to Gradle publications defined in the build script.
            // This is how we tell the Artifactory Plugin which artifacts should be
            // published to Artifactory.
            publications('mavenJava')
            publishArtifacts = true
            // Properties to be attached to the published artifacts.
            properties = ['qa.level': 'basic', 'dev.team' : 'core']
            // Publish generated POM files to Artifactory (true by default)
            publishPom = true
        }
    }
}

task StartTask = {
    // ...
}
```