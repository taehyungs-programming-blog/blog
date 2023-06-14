---
title: "Kubernetes : Kubectl"
permalink: kubernetes/kubectl/                # link 직접 지정
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
  - kubectl
header:
  teaser: /file/image/etc-page-teaser.gif
---

docker desktop이 설치되면 kubectl은 자동으로 설치된다.

간단한 예제를 동작해보자.

```s
$ kubectl run echoserver -- generator-run-pod/v1 --image"k8s.gcr.io/echoserver:1.10" --port==8080
$ kubectl expose po echoserver --type=NodePort
$ kubectl get pods
```

![](/file/image/kubernetes-kubectl-01.png)

간단한 예제를 동작해보자. 2

```s
$ kubectl run nginx-app --image nginx --port=80
$ kubectl get pods
$ kubectl get deployments
```

![](/file/image/kubernetes-kubectl-02.png)

클러스터 외부에서 클러스터 내부 앱에 접근도 가능하다

```s
$ kubectl expose deployment nginx-app --type=NodePort
$ kubectl get service
```

![](/file/image/kubernetes-kubectl-03.png)

```s
$ kubectl describe service nginx-app
```

![](/file/image/kubernetes-kubectl-04.png)

http://localhost:31015/ 주소에 접속하자

![](/file/image/kubernetes-kubectl-05.png)