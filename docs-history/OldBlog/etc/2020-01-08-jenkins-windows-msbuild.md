---
title: "(Jenkins) Windows기반 MSbuild Server 만들기"
date: 2020-01-08 00:00:00 -0000
---

* [참고사이트](https://medium.com/sunhyoups-story/%EC%A0%A0%ED%82%A8%EC%8A%A4%EB%A5%BC-github%EC%99%80-%EC%97%B0%EB%8F%99%ED%95%98%EC%97%AC-msbuild%EB%A1%9C-%EB%B9%8C%EB%93%9C%ED%95%98%EB%8A%94-%EB%B0%A9%EB%B2%95-bd7a8713ea4)

---

### 목차

* [Jenkins 다운로드](#Jenkins-다운로드)
* [Jenkins-MSBuild-Plug-in-설치](#Jenkins-MSBuild-Plug-in-설치)
* [Jenkins Item 등록](#Jenkins-Item-등록)
* [추가로 알게된 사항](#추가로-알게된-사항)

---

### Jenkins 다운로드

* [홈페이지](https://jenkins.io/)에서 Download
* 다운로드와 설치는 쉽기에 생략

> 설치 후 첫 로그인 시에 plug-in을 설치해야하는데 디폴드로 설치하자(아직은 어떤게 필요한지 모르니)

---

### Jenkins-MSBuild-Plug-in-설치

* http://localhost:8080 로 접속하자.

> Jenkins 관리 -> 플러그인 관리 -> 설치 가능에서 MSBuild 설치

![](/file/image/Jenkins_MSbuild_Image_02.png)

> Jenkins 관리 -> Global Tool Configuration -> MSBuild

![](/file/image/Jenkins_MSbuild_Image_03.png)

---

### Jenkins Item 등록

![](/file/image/Jenkins_MSbuild_Image_01.png)

> Jenkins -> 새로운 아이템 -> 아이템 이름적고 -> Freestyle project 클릭

![](/file/image/Jenkins_MSbuild_Image_04.png)

> Item에 접속 -> 구성 -> Build 및 git 등록

![](/file/image/Jenkins_MSbuild_Image_05.png)

---

## 추가로 알게된 사항

### git repo credentials 등록

> 아이템선택 → 구성 → 소스 코드 관리

![](/file/image/Jenkins_MSbuild_Image_06.png)

> 안넣으면 git에 접속이 안됨..(당연한듯?)

---

### 빌드위치 직접 지정

> repo 최 상위에 .sln이 있으면 되지만 없다면 직접 지정 가능
> 
> 추가적으로 `C:\Program Files (x86)\Jenkins\workspace`에 jenkins에 clone한 repo위치함

![](/file/image/Jenkins_MSbuild_Image_07.png)

---

### 빌드 후 배포

> Publish over ssh(무엇이듯) plug-in 설치해줘야함.
> 
> 서버가 없어서 배포는 안해봄 어렵진 않은듯?

![](/file/image/Jenkins_MSbuild_Image_08.png)

![](/file/image/Jenkins_MSbuild_Image_09.png)

---

### CMake and qt build

> CMake의 경우 plug-in을 별도로 설치해줘야한다.
> 
> qt의 경우 window기반의 jenkins서버를 사용하기에 qt가 설치된 위치를 지정해 주면됨.
> 
> Example:
> 
> `C:\Qt\Qt5.13.1\5.13.1\msvc2017_64\bin`

![](/file/image/Jenkins_MSbuild_Image_10.png)

> obs studio 기준으로 build arguments는 
>
> `-S ./obs-studio -B ./obs-studio/build -G "Visual Studio 15 2017 Win64" -DENABLE_SCRIPTING:BOOL="0" -DCOMPILE_D3D12_HOOK:BOOL="1" -DDepsPath:PATH="C:\Program Files (x86)\Jenkins\workspace\jenkins-cmake-test\obs-studio-dependencies2017\win64\include" -DQTDIR:PATH="C:\Qt\Qt5.13.1\5.13.1\msvc2017_64"`

---

### Jenkins 빌드유발 build periodically 설정

분 시간 날짜 월 요일 명령

순으로 진행


예시 케이스

* @ 매일 오전 3시 실행

`00 03 * * *`


* @ 매월 1일 오후 3시 30분 실행

`30 15 1 * *`


* @ 수요일 새벽 2시 실행

`00 02 * * 3`

(월 : 1, 화 : 2, 수 : 3 ... 일 : 7)


* @ 30분마다 실행

`/30 * * * *`


* @ 3시, 8시에 실행

`3,8 * * *`


* @ 3시 ~5시 사이 20분마다 실행

`/20 3-5 * * *`