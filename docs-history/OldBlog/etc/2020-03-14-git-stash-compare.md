---
title: "(git) sourcetree이용 스태시와 특정 커밋 비교"
permalink: git/stash-compare/ # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-14 00:00:00 -0000
last_modified_at: 2020-03-14 00:00:00 -0000
---

> 스태시는 알거라 가정하고 강의를 진행한다.

우선 스태시와 비교할 커밋의 종류는 두 가지가 있을것이다.<br>
스태시를 기준으로 

* 과거의 커밋
* 미래의 커밋

두 가지를 비교하는법을 알려준다.

---

## 미래의 커밋과 스태시 병합

커밋이 아래과 같이 진행되었다고 가정하자<br>
그리고 커밋은 현재 헤드를 기준으로 과거에 존재한다.<br>

![](/file/image/git-stash-compare-image-01.png)

우선 저장된 스태시로 돌려보겠다.<br>

![](/file/image/git-stash-compare-image-02.png)

과거 스태시로 돌리려하니 위와같은 충돌이 발생한다.<br>
Beyond Compare로 과거 충돌을 해결해보자.<br>

![](/file/image/git-stash-compare-image-03.png)

![](/file/image/git-stash-compare-image-04.png)

이런식으로 병합이 가능하다

---

## 과거/미래의 커밋과 스태시 비교만

비교만 할 경우 스태시로 아에 돌아가야한다.<br>
헤드의 위치를 스태시 커밋으로 변경 후 스태시를 복원시킨 후 비교하면되겠지? (말은 쉽다 자세한 설명은 아래에)

![](/file/image/git-stash-compare-image-05.png)

위 상황은 스태시 커밋으로 헤드를 변경하고, 스태시를 부른 상황이다<br>
여기서 내가 비교하고 싶은 커밋을 클릭만 하면 된다.

![](/file/image/git-stash-compare-image-06.png)