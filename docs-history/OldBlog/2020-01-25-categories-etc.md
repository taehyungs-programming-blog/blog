---
title: "기타사항 정리 페이지 입니다."
permalink: etc/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-01-04 00:00:00 -0000
last_modified_at: 2020-05-09 00:00:00 -0000
sidebar:
  title: "etc 목차"
  nav: etc
tag:
  - 기타사항
category:
  - category
classes: wide
excerpt: "기타사항 정리 페이지 입니다"
header:
  teaser: /file/image/etc-page-teaser.gif
  overlay_image: /file/image/main-page.jpg
  overlay_filter: 0.1 # rgba(255, 0, 0, 0.5)
  caption: "Photo credit: [**EBS**](https://ebs.co.kr)"
---

## UML

* [Class Diagram](/etc/uml/class-diagram/)

---

## Window Pro.

* [Process Explorer](/winp/pe/)
* [Windows Service OpenSSL 확인](/winp/service-openssl/)

---

## ffmpeg

* [ffmpeg 기본적 사용법 + hw-encoding](/ffmpeg/hwenc/)

---

## jFrag artifactory

* [install](/jFrag/install/)
* [jenkins 연동](/jFrag/jenkins/)
* [generic build upload](/jFrag/generic/)

---

## Hyper-V & Linux

> 언제 VirtualBox를 언제 Hyper-V를 쓰면 좋나?<Br>
> Linux로 GUI 작업을 해야 한다면? 무조건 VirtualBox<Br>
> SSH접속만으로 처리가 가능하다면 Hyper-V<Br>

* [Linux 과거 자료](https://github.com/8BitsCoding/RobotMentor/wiki/Linux_Ubuntu)

* [Hyper-V 해상도 변경](/hyper-v/resolution/)
* [Ubuntu 16.04 file browser에서 open terminal](/linux/openterminal/)
* [Hyper-V VNX](/linux/vnc/)
* [Linux Samba(공유폴더) 사용하기](/linux/samba/)

## Kubernetes

* [Kubernetes 설치하기](/kubernetes/install/)
* [Kubernetes 도구활용](/kubernetes/tools/)
* [Kubectl](/kubernetes/kubectl/)

---

## gradle

* [강의](https://www.youtube.com/watch?v=s-XZ5B15ZJ0&list=PL7mmuO705dG2pdxCYCCJeAgOeuQN1seZz)

* [gradle이란?](/gradle/basic/)
* [설치](/gradle/install/)
* [gradle vs maven](/gradle/gradle-maven/)
* [build](/gradle/build/)
* [task](/gradle/task/)

* [task graph](/gradle/task-graph/)
* [객체](/gradle/object/)
* [파일관리](/gradle/files/)

여기서 부터는 완전히 정리하지 않음 필요없는 부분이 너무 많음 필요할 경우 찾아서 볼 것

* [의존관계](/gradle/dependencies/)

### Example

* [.bat 파일 실행하기](/gradle/example/startbat/)
* [delete file and folder](/gradle/example/delete/)
* [특정 폴더에 파일만 복사하기](/gradle/example/copyfiles/)

### gradle plugin

* [gitpublish](/gradle/plugin/gitpublish/)

---

## Docker

* [Docker file 문법 정리](https://8bitscoding.github.io/docker-syntax)
* [mac기반 Docker 기본적 사용법](https://8bitscoding.github.io/docker-mac-basic)
* [Docker WebServer](https://8bitscoding.github.io/docker-linux-webdav) : WebDav, Transmission(Torrent), Plex Server

---

## Jenkins

* [Git SSH Agent가 잘 동작하지 않을 시 환경변수로 SSH-Key 넣기](/jenkins/env-ssh-key/)

* [Windows 기반 MSBuild Jenkins 서버만들기](https://8bitscoding.github.io/jenkins-windows-msbuild/)
* [Jenkins Window Batch Good Example : MSBuild](/jenkins/windowbatch-goodexample/)
* [Jenkins + gradle plugin](/jenkins/gradleplugin/)
* [Email Notification](/jenkins/emailnoti/)
* [Jenkins WorkSpace에 관해서]() : 간단하기에 글로 정리 - Jenkins가 설치된 폴더의 workspace가 존재 하며 그 폴더에서 git, svn을 clone하여 프로젝트를 빌드한다. 한번씩 헷갈려서 혹시나 해서 적어둠.

* [item Build Count reset](/jenkins/reset-build-cnt/)
* [Jenkins Window Batch 작성하기](/jenkins/windows-batch/) : 그냥 window batch에 작성하면 동작하는게 Jenkins에서 작성하면 동작안하는 경우가 발생함.
* [Jenkins Git 특정 Branch 선택하기]() : `refs/heads/develop`
* [Git Credential(암호) 관리](/jenkins/credential/)
* [Qt VS Tools 연동]()

```s
# Jenkins에서 아래 환경변수를 선언해 줘야한다
set QtDir=C:\Qt\Qt5.13.1\5.13.1\msvc2017_64
set QtInstallDir=C:\Qt\Qt5.13.1\5.13.1\msvc2017_64
set QtMsBuild=C:\Users\th\AppData\Local\QtMsBuild
```

---

## Windows Batch

* [*.rc파일 버전 병경하기](/batch/example-file-replace/)
* [bat curl 쓰기](/batch/curl/)

---

## Github Page

* [jekyll로 github page만들기](https://8bitscoding.github.io/gitpage-make/)
* [테스트 이미지 넣기](/githubio/insert-image/)
* [google analytics 붙이기](https://8bitscoding.github.io/gitpage-analysis/)
* [google adsense 붙이기](https://8bitscoding.github.io/gitpage-adsense/)

* [sample-page](https://8bitscoding.github.io/sample/page-sample/) : sample-page + side-bar navigator
* [sitemap 넣기](/github-io/insert-sitemap/)
* [robots.txt 수정](http://jinyongjeong.github.io/2017/01/13/blog_make_searched/) : `root`아래 robots.txt를 넣으면됨.
* [github page 특수문자 넣기](/githubio/special-char/)
* [_config.yml 수정시 주의사항](/githubio/caution-config/)

* Google Search Console이 제대로 동작하지 않을시 그냥 Github-page를 rename해서 새로 Google Search Console을 등록해 버리는게 차라리 빠름...

---

## Git

* [Git 명령어 정리](/git/command-summary/)
    
* [git의 difftool 지정하기(beyond compare4)](https://8bitscoding.github.io/git-difftool/)
* [Sourcetree로 Git 정리하기](https://8bitscoding.github.io/git-source-tree/)
* [master, develop, hofix 등 git flow 정리](https://8bitscoding.github.io/git-flow/)
* [Sourcetree로 cherry pick하기](https://8bitscoding.github.io/git-cherry-pick/)
* [과거 commit 버전간 비교](https://8bitscoding.github.io/git-compare-past-commit/)

* [sourcetree이용 스태시와 특정 커밋 비교](/git/stash-compare/)
* [sourcetree이용 release 만들기](/git/release/)
* [github ssh 등록](/git/github-ssh/)
* [git ignore 생성](/git/ignore/)
* [Submodule 추가하기](/git/submodule/)

---

## markdown

* [markdown 문법](/markdown/grammer/)

---

## Windows command

* [Window 명령어 정리](/windows/command/)

* [인바운드, 아웃바운드 규칙이란](/windows/inoutbound/)

---

## Visual Studio

* [DLL 디버깅 (Break Point 찍기)](/vs/dll-debug/)
* [각종 설정](/vs/settings/)
* [WinDbg 사용하기](/vs/WinDbg/)
* [빌드 후 파일복사](/vs/buildAftercopy/)
* [최신 C++(C++17 이상) 사용하기](/vs/uselatestcpp/)

* [스크롤 막대 띄우기]()

![](/file/image/vs-scrollbar.jpg)

---

## Visual Studio 추천 plug-in

* [Word Highlight With Margin](https://marketplace.visualstudio.com/items?itemName=TrungKienPhan.WordHighlight-18439)

![](/file/image/WordHighlightWithMargin.jpg)

* [MetaProject](https://marketplace.visualstudio.com/items?itemName=ViacheslavLozinskyi.MetaProject&ssr=false#overview)

TODO 전시기

![](/file/image/MetaProject.gif)

* [Highlight Comment Tags](https://marketplace.visualstudio.com/items?itemName=zloywolk.highlight-comment-tags)

![](/file/image/HighlightCommentTags.png)

---

## Visual Studio Code 추천 plug-in

* [highlight-words](https://marketplace.visualstudio.com/items?itemName=rsbondi.highlight-words)

![](/file/image/highlight-words.gif)

- `Ctrl + Shifht + p` -> Highlight toggle Current

* [Text Marder](https://marketplace.visualstudio.com/items?itemName=ryu1kn.text-marker)

* [GitLens](https://marketplace.visualstudio.com/items?itemName=eamodio.gitlens)

- 개좋음

![](/file/image/git-lens.gif)

![](/file/image/git-lens-01.png)

각 줄별로 git history를 분석할 수 있음

![](/file/image/git-lens-02.png)

git commit별로 비교할 수 있음

* [platUML](http://snowdeer.github.io/tips/2019/10/04/visual-studio-code-how-to-use-uml/)

```
@startuml

scale 2
title My First Diagram

A -> B : Hello
B -> C : Good Morning
C --> A : Bye Bye

@enduml
```

Shift + Command + p 를 눌러서 PlantUML: Preview Current Diagram을 선택

![](/file/image/platUML.png)

* 동시편집 : Ctrl + Alt + Shift

---

## curl

* [설치](/curl/install/)

---

## websocket

* [참고사이트](http://utk-unm.blogspot.com/2016/10/websocket.html)

* WebSocket
    - Transport protocol 일종(Real-time web application을 지원(서버와 실시간 통신지원))
    - Web의 TCP Socket이라 생각하자

* 대표적 사용예
    - 구글 Doc과 같이 여러명 동시 수정 웹
    - 실시간 스포츠 업데이트 사이트

---

## stomp

Simple (or Streaming) Text Orientated Messaging Protocol.<br>
말 그대로 메시징 프로토콜이다.<br>
websocket에서 사용되며 특정 규약에 맞춰 보낼시 파싱<br>

서버에 구독을 요청하고 메시지가 오면 받아서 파싱한다. 통신의 기반은 WebSocket이고 메시지는 json을 쓴다.

* [참고사이트](https://swiftymind.tistory.com/tag/Websocket%20%2B%20STOMP)
* [참고사이트2 - 설명 좋음](https://supawer0728.github.io/2018/03/30/spring-websocket/)

---

## powershell 7

* [GitHub(다운로드)](https://github.com/PowerShell/PowerShell/releases)

---

## startuml

startuml 1.0 다운로드

* [다운로드 페이지](http://sourceforge.net/projects/staruml/files/staruml/5.0/)

![](/file/image/startuml.png)

---

## YOLO

* [과거자료](https://github.com/8BitsCoding/RobotMentor/wiki/YOLO)

---

## Python

* [과거자료](https://github.com/8BitsCoding/RobotMentor/wiki/Python)

---

## Java

### Java 기초

* [basic 1 : 주석, Console print, 변수할당, 변수이름 규칙](/java/basic-1/)
* [basic 2 : 자료형 정리, 상수, 자료형변환, 연산자](/java/basic-2/)
* [basic 3 : 반복문](/java/basic-3/)
* [basic 4 : 배열](/java/basic-4/)

---

## Android

### Tutorial

* [Tutorial - 1](/android/tutorial-1/) : 프로젝트 생성하기, 가상머신 실행하기, 버튼 생성하기
* [Tutorial - 2](/android/tutorial-2/) : 버튼만들기, 함수연결하기
* [Tutorial - 3](/android/tutorial-3/) : Layout 변경방법, LinearLayout

### TroubleShoot

#### 에뮬레이터가 실행안되는 경우

64bit이 보함된 에뮬레이터를 설치하고. 그래픽도 수정

![](/file/image/Android-TroubleShoot-1.png)