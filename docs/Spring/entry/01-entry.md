---
layout: default
title: "01. 환경설정"
parent: "(입문)"
grand_parent: "Spring 🐍"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* JAVA 설치
* IntelliJ Community 설치
* https://start.spring.io/ 에 접속 후 아래와 같이 설정

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/entry/entry-1-1.png"/>
</p>

🐍 Intelli J에서 Open하는 것 만으로도 Lib가 모두 설치된다(시간걸림)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/entry/entry-1-2.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/entry/entry-1-3.png"/>
</p>

🐍 Tips) Build, Run 속도가 느리다면 ?? 👉 File -> Settings -> gradle 검색후 아래를 변경.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/entry/entry-1-4.png"/>
</p>

---

🐍 Spring의존성 확인

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/entry/entry-1-5.png"/>
</p>

### 스프링 부트 라이브러리

* spring-boot-starter-web
  * spring-boot-starter-tomcat: 톰캣 (웹서버)
  * spring-webmvc: 스프링 웹 MVC
* spring-boot-starter-thymeleaf: 타임리프 템플릿 엔진(View) 
* spring-boot-starter(공통): 스프링 부트 + 스프링 코어 + 로깅 
* spring-boot 
  * spring-core 
* spring-boot-starter-logging 
  * logback, slf4j 
  
### 테스트 라이브러리 
* spring-boot-starter-test 
  * junit: 테스트 프레임워크 
  * mockito: 목 라이브러리 
  * assertj: 테스트 코드를 좀 더 편하게 작성하게 도와주는 라이브러리 
  * spring-test: 스프링 통합 테스트 지원

🐍 일단은 이런게 있다 정도만 알아두자.