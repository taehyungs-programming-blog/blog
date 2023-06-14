---
title: "Kubernetes : Kubernetes 설치하기"
permalink: kubernetes/install/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-06-07 00:00:00 -0000
last_modified_at: 2020-06-07 00:00:00 -0000
sidebar:
  title: "etc"
  nav: etc
excerpt: ""
tag:
  - Kubernetes
category:
  - install
header:
  teaser: /file/image/etc-page-teaser.gif
---

## 설치없이 Kubernetes 사용하기

### 방법 1 - 카타코다 쿠버네티스

* [카타코다 쿠버네티스](https://www.katacoda.com/courses/kubernetes)

![](/file/image/Kubernetes-install-01.png)

course를 진행하며 배우는 kubernetes를 사용해보는 과정인듯.<br>
이것보다는 직접 설치를 해보는 것을 추천. 설치해보는 것도 하나의 배움.<br>

### 방법 2 - Play with Kubernetes

* [Play with Kubernetes](https://labs.play-with-k8s.com/)

![](/file/image/Kubernetes-install-02.png)

역시 직접설치해 보는 것이 낫다.

---

## 설치

* [다운로드 사이트](https://hub.docker.com/editions/community/docker-ce-desktop-windows)

![](/file/image/Kubernetes-install-03.png)

실행파일 다운로드가 진행된다.

![](/file/image/Kubernetes-install-04.png)

power shell에서

```s
$ docker version
```

으로 설치확인가능

Kubernetes 적용

![](/file/image/Kubernetes-install-05.png)

```s
$ kubectl version
```

![](/file/image/Kubernetes-install-06.png)

```s
$ kubectl get pods -n kube-system
```

![](/file/image/Kubernetes-install-07.png)