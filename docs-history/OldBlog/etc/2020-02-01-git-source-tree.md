---
title: "(git) Sourcetree로 Git 정리하기"
date: 2020-02-01 00:00:00 -0000
---

## 목차

> 기초적 내용

* [clone 해보자](#clone-해보자)
* [commit 해보자](#commit-해보자)
* [push 해보자](#push-해보자)
* [pull 해보자](#pull-해보자)

> 여기서 부터 심화

* [Branch에 관해서](#Branch에-관해서)

> 생활코딩 - git-sourcetree 내용정리

* [저장소 만들기(init)](#저장소-만들기(init))
* [아이디/email 설정 및 첫 번째 commit](#아이디/email-설정-및-첫-번째-commit)
* [working copy, add, index, staging area 개념](#working-copy,-add,-index,-staging-area-개념)
* [(commit 이전) 수정사항 취소하기](#(commit-이전)-수정사항-취소하기)
* [(commit 이후) Reset(hard)](#(commit-이후)-Reset(hard))
* [(commit을 삭제하지 않고 이전 commit으로 돌아감) Revert](#(commit을-삭제하지-않고-이전-commit으로-돌아감)-Revert)
* [Branch 만들기](#Branch-만들기)
* [Branch 합치기 및 충돌 해결](#Branch-합치기-및-충돌-해결)
* [충돌을 예방하고 싶다면?](#충돌을-예방하고-싶다면?)
* [원격저장소(Github) 활용](#원격저장소(Github)-활용)
* [협업해보기](#협업해보기)
* [협업 중 충돌을 방지하려면?](#협업-중-충돌을-방지하려면?)

> 조금 더 고급 주제

* [차이점 비교 도구](#차이점-비교-도구)
* [차이점 비교 도구 충돌 상황 대처](#차이점-비교-도구-충돌-상황-대처)
* [Stash](#Stash) : 지금까지 해왔던 코드를 남겨두고 다른 작업을 하고 싶다.
* [Tag](#Tag)
* [gitignore 적용](#gitignore-적용)
* [환경파일의 관리](#환경파일의-관리)

---

## clone 해보자

![](/file/image/Git_sourcetree_Image_01.png)

> 너무 쉽기에 설명은 생략한다...

---

## commit 해보자

![](/file/image/Git_sourcetree_Image_02.png)

![](/file/image/Git_sourcetree_Image_03.png)

> 여기도 너무 쉽기에 생략...

---

## push 해보자

> 그냥 push 버튼 누르면 됨..

---

## pull 해보자

> 그냥 pull 버튼 누르면 됨..

![](/file/image/Git_sourcetree_Image_04.png)

---

## Branch에 관해서

> 현재 branch는 다음과 같이 세 가지가 있다.
>
> master, origin/master, origin/HEAD

![](/file/image/Git_sourcetree_Image_05.png)

* master : 가장 초기에 생성되는 branch 보통 여기에 release 버전의 소스를 넣는다.
* HEAD : HEAD는 현재 내가 선택한 branch를 의미한다.

> * [참고사이트](https://backlog.com/git-tutorial/kr/stepup/stepup1_1.html)

---

## 저장소 만들기(init)

![](/file/image/Git_sourcetree_Image_06.png)

![](/file/image/Git_sourcetree_Image_07.png)

![](/file/image/Git_sourcetree_Image_08.png)

---

## 아이디/email 설정 및 첫 번째 commit

> 아이디/비밀번호 정보를 먼저 입력해 둬야함.

![](/file/image/Git_sourcetree_Image_10.png)

> 쉽기에 내용은 생략

![](/file/image/Git_sourcetree_Image_09.png)

---

## working copy, add, index, staging area 개념

![](/file/image/Git_sourcetree_Image_11.png)

---

## (commit 이전) 수정사항 취소하기

> commit 이전 특정 내용을 변경했는데 다시 되 돌리고 싶다면??

![](/file/image/Git_sourcetree_Image_12.png)

![](/file/image/Git_sourcetree_Image_13.png)

> commit 한 당시로 돌려보내 준다.

---

## (commit 이후) Reset(hard)

> 해당 commit 우클릭 -> 이 커밋까지 현재 브랜치 초기화 -> hard 선택

![](/file/image/Git_sourcetree_Image_14.png)

![](/file/image/Git_sourcetree_Image_15.png)

![](/file/image/Git_sourcetree_Image_16.png)

> 상위에 commit 내용이 모두 날아간다.

---

## (commit 이후) Reset(mixed)

> commit 정보를 없애고 현재 working copy의 내용은 남기고 싶을 때.

> 해당 commit 우클릭 -> 이 커밋까지 현재 브랜치 초기화 -> mixed 선택

![](/file/image/Git_sourcetree_Image_17.png)

> 내용 추가 1, 2 commit은 날리고 싶고 현재 working copy의 내용은 남기고 싶다

![](/file/image/Git_sourcetree_Image_18.png)

![](/file/image/Git_sourcetree_Image_19.png)

---

## (commit을 삭제하지 않고 이전 commit으로 돌아감) Revert

> commit을 삭제하지 않으면서 예전 commit으로 돌아가고 싶다면?

![](/file/image/Git_sourcetree_Image_20.png)

> 단, 한 번에 여러 commit을 이동하게 되면 충돌이 발생하기에 순차적으로 revert를 진행한다.

![](/file/image/Git_sourcetree_Image_21.png)

## Branch 만들기

![](/file/image/Git_sourcetree_Image_22.png)

> 브랜치 버튼을 통해서 브랜치 생성이 가능

> 여기서 중요한거는 좌측의 master, develop을 고르면 코드가 달라진다는 것을 기억하자.

![](/file/image/Git_sourcetree_Image_23.png)

---

## Branch 합치기 및 충돌 해결

> 하고 싶은게 master에서 develop의 내용을 추가하는 거겠지??

> Branch를 master로 이동 후 병합을 누른다.

![](/file/image/Git_sourcetree_Image_24.png)

![](/file/image/Git_sourcetree_Image_25.png)

> 충돌이 자주 발생하게 된다.

![](/file/image/Git_sourcetree_Image_26.png)

> 충돌 상황을 확인 후 git에게 해결했다는 것을 알려줘야한다.

![](/file/image/Git_sourcetree_Image_27.png)

> 다시 commit을 날리면된다.

![](/file/image/Git_sourcetree_Image_28.png)

---

## 충돌을 예방하고 싶다면?

* 주기적으로 병합을 진행한다.

---

## 원격저장소(Github) 활용

> 원격저장소와 로컬저장소를 연결하자

![](/file/image/Git_sourcetree_Image_29.png)

![](/file/image/Git_sourcetree_Image_30.png)

![](/file/image/Git_sourcetree_Image_31.png)

> push를 해본다.

![](/file/image/Git_sourcetree_Image_32.png)

![](/file/image/Git_sourcetree_Image_33.png)

![](/file/image/Git_sourcetree_Image_34.png)

> 각 Branch의 위치를 확인하자

> 깃 헙에 push이후

![](/file/image/Git_sourcetree_Image_35.png)

---

## 협업 중 충돌을 방지하려면?

> 역시 pull/병합을 자주 한다.

> 한 번 충돌을 만들어보자.

![](/file/image/Git_sourcetree_Image_36.png)

![](/file/image/Git_sourcetree_Image_37.png)

![](/file/image/Git_sourcetree_Image_38.png)

---

## 차이점 비교 도구

> 수업에서는 [beyond compare](http://www.scootersoftware.com/)를 사용

> 유료이긴하나 돈 내고 쓸만하다는 평...

> 우선 sourcetree에서 diff를 기본 실행프로그램을 설정한다.

![](/file/image/Git_sourcetree_Image_39.png)

![](/file/image/Git_sourcetree_Image_40.png)

---

## 차이점 비교 도구 충돌 상황 대처

![](/file/image/Git_sourcetree_Image_41.png)

![](/file/image/Git_sourcetree_Image_42.png)

![](/file/image/Git_sourcetree_Image_43.png)

> 가장 아래 창은 내가 수정할 창
>
> 가운데 창은 공통점 창
>
> 왼/오른쪽 창은 각 commit화면

![](/file/image/Git_sourcetree_Image_44.png)

![](/file/image/Git_sourcetree_Image_45.png)

---

## Stash

> Stash(안전한 곳에 보관해 두다.) :
>
> 지금까지 해왔던 코드를 남겨두고 다른 작업을 하고 싶다.

![](/file/image/Git_sourcetree_Image_46.png)

![](/file/image/Git_sourcetree_Image_47.png)

![](/file/image/Git_sourcetree_Image_48.png)

---

## Tag

> 버전들 안에서 의미있는 버전을 관리

![](/file/image/Git_sourcetree_Image_49.png)

![](/file/image/Git_sourcetree_Image_50.png)

![](/file/image/Git_sourcetree_Image_51.png)

> 원격저장소에 push할때 tag정보를 같이 넣어주기 위해서 꼭 선택하자.

---

## gitignore 적용

![](/file/image/Git_sourcetree_Image_52.png)

![](/file/image/Git_sourcetree_Image_53.png)

![](/file/image/Git_sourcetree_Image_54.png)

> ignore할 파일을 관리한다.

> * [gitignore.io](http://gitignore.io/)

---

## 환경파일의 관리

> * [강의](https://opentutorials.org/course/1492/8129)