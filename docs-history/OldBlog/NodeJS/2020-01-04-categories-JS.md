---
title: "NodeJS"
permalink: NodeJS/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-01-04 00:00:00 -0000
last_modified_at: 2020-05-09 00:00:00 -0000
header:
  teaser: /file/image/nodejs-page-teaser.gif
sidebar:
  title: "NodeJS" 
  nav: NodeJS
tag:
  - NodeJS
category:
  - category
excerpt: "NodeJS와 관련된 지식을 정리한 페이지 입니다."
header:
  teaser: /file/image/qt-page-teaser.gif
  overlay_image: /file/image/main-page.jpg
  overlay_filter: 0.1 # rgba(255, 0, 0, 0.5)
  caption: "Photo credit: [**EBS**](https://ebs.co.kr)"
  actions:
    - label: "NodeJS HomePage"
      url: "https://nodejs.org/"
---

## HTML 테그 정리

* [HTML 테그 정리](/vuejs/html-tag/)

---

## Vue.JS : 상급

* [Vue.JS 상급 1](/VueJS/adv-1/) : 프로젝트 설정 / 셋업
* [Vue.JS 상급 2](/VueJS/adv-2/) : 라우터
* [Vue.JS 상급 3](/VueJS/adv-3/) : API
* [Vue.JS 상급 4](/VueJS/adv-4/) : Mutations, State
* [Vue.JS 상급 5](/VueJS/adv-5/) : 라우터2

여기서부턴 Example 더 필요할 경우 정리

* [Vue.JS 상급 6](/VueJS/adv-6/) : 리팩토링1
* [Vue.JS 상급 7](/VueJS/adv-7/) : 리팩토링2

---

## WebPack

* [1강](/webpack/1/) : 개발환경구성
* [2강](/webpack/2/) : npm install
* [3강](/webpack/3/) : webpack 세팅
* [4강](/webpack/4/) : webpack 사용
* [5강](/webpack/5/) : babel과 ES6

* [6강](/webpack/6/) : webpack의 주요 속성
* [7강](/webpack/7/) : webpack dev server

---

## Vue.JS : 기초

* [Vue.JS 강의 기초 1 - 1](/VueJS/1-1/) : Vue를 배우기 위한 준비
* [Vue.JS 강의 기초 2](/VueJS/1-2/) : Vue소개, MVVM 설명
* [Vue.JS 강의 기초 3](/VueJS/1-3/) : Vue 인스턴스 설명
* [Vue.JS 강의 기초 4](/VueJS/1-4/) : 컴포넌트 설명
* [Vue.JS 강의 기초 5](/VueJS/1-5/) : 컴포넌트 통신 방법, props

* [Vue.JS 강의 기초 6](/VueJS/1-6/) : event emit
* [Vue.JS 강의 기초 7](/VueJS/1-7/) : 같은 컴포넌트 레벨간의 통신
* [Vue.JS 강의 기초 8](/VueJS/1-8/) : Vue Router
* [Vue.JS 강의 기초 9](/VueJS/1-9/) : http통신 라이브러리 axios
* [Vue.JS 강의 기초 10](/VueJS/1-10/) : 템플릿 문법

* [Vue.JS 강의 기초 11](/VueJS/1-11/) : Watch
* [Vue.JS 강의 기초 12](/VueJS/1-12/) : Vue CLI
* [Vue.JS 강의 기초 13](/VueJS/1-13/) : Vue CLI 활용하기
* [Vue.JS 강의 기초 14](/VueJS/1-14/) : 배운 지식을 기반으로 한 Example

---

## Vue.JS 중급

* [Get Code](https://github.com/EasyCoding-7/vue-intermediate) : Private Code라 접속불가, 강의를 수강후 받자

### todo-app components-implementation Branch

* [1강](/vuejs/mid-1/) : 강의목표 및 개발환경 구성
* [2강](/vuejs/mid-2/) : 프로젝트 만들기
* [3강](/vuejs/mid-3/) : 컴포넌트 만들기
* [4강](/vuejs/mid-4/) : TodoHeader, TodoInput 구현
* [5강](/vuejs/mid-5/) : TodoInput UI 처리

* [6강](/vuejs/mid-6/) : TodoList.vue
* [7강](/vuejs/mid-7/) : Toggle TodoList.vue
* [8강](/vuejs/mid-8/) : TodoFooter.vue

### todo-app components-refactoring Branch

* [9강](/vuejs/mid-9/) : 리팩토링

### todo-app components-ux Branch

* [10강](/vuejs/mid-10/) : Modal 경고창 나타내기
* [11강](/vuejs/mid-11/) : 트렌지션
* [12강](/vuejs/mid-12/) : ES6, Babel

ES6 문법 설명

* [13강](/vuejs/mid-13/) : ES5 특징 정리(const & let)
* [14강](/vuejs/mid-14/) : const & let 리팩토링
* [15강](/vuejs/mid-15/) : ES5 특징 정리(화살표 함수)
* [16강](/vuejs/mid-16/) : Enhanced Object Literals
* [17강](/vuejs/mid-17/) : Modules

Vuex

* [18강](/vuejs/mid-18/) : Vuex

### es6 const-let Branch

* [19강](/vuejs/mid-19/) : Vuex 설치, state와 getters 소개
* [20강](/vuejs/mid-20/) : mutations, commit
* [21강](/vuejs/mid-21/) : actions

Helper function

* [22강](/vuejs/mid-22/) : helper
* [23강](/vuejs/mid-23/) : mapState, mapGetters
* [24강](/vuejs/mid-24/) : mapMutations, mapActions
* [25강](/vuejs/mid-25/) : 헬퍼의 편리함 정리

스토어 모듈화

* [26강](/vuejs/mid-26/)

---

## Vue.JS 대표 Example : StreamLabs

UI를 어떻게 그리는지만 살펴본다

* [GitHub](https://github.com/stream-labs/streamlabs-obs)

* [StreamLabs 빌드하기](/vuejs/streamlabs/)

---

## Electron 기초

우선 ... Electron 자체가 Web을 개발자들이 쉽게 네이티브로 넘어오길 바라면서 만들어진 프레임 워크이기에 Web개발에 대한 지식을 바탕해야한다.<br>
간단한 사용법 정도는 이해하지만 깊이있는 개발을 위해서는 Web개발을 공부해 보자.<br>

* [Electron이란?](/NodeJS/Electron/1/)
* [프로젝트 생성 및 어플리케이션 실행, 종료시 호출함수 분석](/NodeJS/Electron/2/) : app.on, app.quit 등
* [BrowserWindow 모듈 분석](/NodeJS/Electron/3/) : BrowserWindow 띄우기, 여러개 띄우기, 자연스럽게 띄우기
* [BrowserWindow event 처리](/NodeJS/Electron/4/)
* [Munu 모듈 알아보기](/NodeJS/Electron/5/) : 상위 메뉴 처리, 우클릭 시 메뉴 처리
* [Tray 모듈 알아보기](/NodeJS/Electron/6/)
* [dialog 모듈 알아보기](/NodeJS/Electron/7/)
* [ipcMain, ipcRenderer 모듈 알아보기](/NodeJS/Electron/8/) : 프로세스간 통신 / 동기, 비동기 방식의 ipc처리
* [remote, shell, process 모듈 알아보기](/NodeJS/Electron/9/)

* [북마크 앱 만들기1](/NodeJS/Electron/10/)

![](/file/image/E2-10-8.png)

* [북마크 앱 만들기2](/NodeJS/Electron/11/)

![](/file/image/E2-11-5.png)

* [북마크 앱 만들기3](/NodeJS/Electron/12/)
* [북마크 앱 만들기4](/NodeJS/Electron/13/)

![](/file/image/E2-13-2.png)

* [북마크 앱 만들기5](/NodeJS/Electron/14/)
* [북마크 앱 만들기6](/NodeJS/Electron/15/) : app의 Renderer process를 React로 변경하기

---

## Electron으로 채팅앱 만들기

### 로그인

* [IDE & Babel](/NodeJS/Elect/1/) : WebStorm이용 Babel 설치
* [express Backend](/NodeJS/Elect/2/) : 

강좌 이상해서 정리안함. -> 필요할 경우 다시 가서 볼 것

---

## Node.js 생활 코딩 강의 정리

* [**생활코딩 강의 홈페이지**](https://opentutorials.org/course/3332) : 생활코딩 Node.js

* [코드를 정리한 Git](https://github.com/8bitscoding/Nodejs)
* [1강 : Node.js설치](/nodejs/1/) : 설치 및 확인
* [참고) Vue.js 개발자 도구 다운로드](https://chrome.google.com/webstore/detail/vuejs-devtools/nhdogjmejiglipccpnnnanhbledajbpd)

* [2강 : Node.js 웹서버 만들기](/nodejs/2/) : Node.js Example Server 동작해보기
* [JavaScript - Data type Number](/nodejs/3/) : Number 출력
* [JavaScript - Data type String](/nodejs/4/) : String 출력
* [JavaScript - 변수](/nodejs/5/)

* [JavaScript - Template Literal](/nodejs/6/) : or template string
* [7강 : URL로 입력된 값 사용하기](/NodeJS/basic-07/) : Query String, URL 정의
* [8강 : 동적인 웹페이지 만들기](/NodeJS/basic-08/)
* [9강 : 파일 읽기](/NodeJS/basic-09/) : `fs.readFile`
* [10강 : 파일을 이용해 본문 구현](/NodeJS/basic-10/)

* [11강 : Node.js 파일 읽기](/NodeJS/basic-11/) : `readFile`활용
* [12강 : App 파일을 이용해 본문 구현](/NodeJS/basic-12/)
* [13강 : JS Boolean](https://opentutorials.org/course/3332/21050) : (별도정리없음) `console.log(true);`, `console.log(false);`
* [14강 : JS 비교연산자](https://opentutorials.org/course/3332/21059) : (별도정리없음) `console.log(1==1); //true`
* [15강 : JS 제어문](https://opentutorials.org/course/3332/21060) : (별도정리없음)

* [16강 : JS 조건문](https://opentutorials.org/course/3332/21061) : (별도정리없음) `if(false) // ~`
* [17강 : NodeJS 콘솔에서의 입력값](/NodeJS/basic-17/)
* [18강 : Not found 오류 구현](/NodeJS/basic-18/)
* [19강 : App - 홈페이지 구현](/NodeJS/basic-19/)
* [20강 : JS 반복문](https://opentutorials.org/course/3332/21119) : (별도정리없음) `while(i < 2)`

* [21강 : JS 배열](https://opentutorials.org/course/3332/21120) : (별도정리없음) `var arr = ['A','B','C','D'];`
* [22강 : JS 배열과 반복문](/NodeJS/basic-22/)
* [23강 : NodeJS 파일 목록 알아내기](/NodeJS/basic-23/)
* [24강 : App - 글목록 출력하기](/NodeJS/basic-24/)
* [25강 : JS 함수의 기본문법](/NodeJS/basic-25/) : `function f123(){`

* [26강 : JS 함수의 입력](/NodeJS/basic-26/)
* [27강 : JS 함수의 출력](/NodeJS/basic-27/)
* [28강 : App - 함수를 이용해서 정리 정돈하기](/NodeJS/basic-28/)
* [29강 : NodeJS - 동기와 비동기 그리고 콜백](/NodeJS/basic-29/)
* [30강 : NodeJS - 패키지 매니저와 PM2](/NodeJS/basic-30/) : npm에 대해서

* [31강 : HTML - Form](/NodeJS/basic-31/)
* [32강 : App - 글생서 UI만들기](/NodeJS/basic-32/)
* [33강 : App - POST 방식으로 전송된 데이터 받기](/NodeJS/basic-33/)

> 더 이상 정리하지 않음. 필요하다 생각시 추가로 정리할 것.

---

## JavaScript 문법 기본강의 정리

* [JavaScript Basic](/NodeJS/JS-basic/)

---

## Intelli-J 단축키 쓰기

* [강의](https://www.youtube.com/watch?v=0D-YYVfUxFM)

우선 Key Promoter X를 다운 받는다.

![](/file/image/Intelli-j.png)

이후 특정동작을 하면 Key Promoter X가 그 동작의 단축키를 알려준다.

![](/file/image/Intelli-j2.png)

---

### 강의) Electron for Desktop Apps: The Complete Developer's Guide

> Secion 1. Basics of Electron

> Section 1은 이론적 부분.

* [Section 2. - Handling Electron Projects](https://8bitscoding.github.io/Electron-2-1/)
* [Section 3. - Handling Menu Bars](https://8bitscoding.github.io/Electron-3/)
* [Section 4. - Status Tray Applications(1/2)](https://8bitscoding.github.io/Electron-4/)
* [Section 4. - Status Tray Applications(2/2)](https://8bitscoding.github.io/Electron-4-2/) : 길이가 길어져 나눔, 같은 내용임.
* [Section 5. - Combining Electron with React and Redux](https://8bitscoding.github.io/Electron-5/) : 뒷 쪽부터 잘 이해가 되지 않아 강의 중지..