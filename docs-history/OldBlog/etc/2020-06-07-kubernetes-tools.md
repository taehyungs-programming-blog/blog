---
title: "Kubernetes : Kubernetes 도구활용"
permalink: kubernetes/tools/                # link 직접 지정
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
  - tools
header:
  teaser: /file/image/etc-page-teaser.gif
---

> 다양한 tool이 존재하지만 예제에는 구글 쿠버네티스를 활용할 예정(아마존, 마이크로소프트 모두 이런 툴을 제공한다.)

* [구글 쿠버네티스 엔진](https://cloud.google.com/kubernetes-engine)

> 이게 이상한게 우리나라 ip에서는 접속이 안될때가 있음 vpn을 통해서 다른 나라 ip를 통해 접속해서 가입하자

클러스터 만들기

![](/file/image/kubernetes-tools-01.png)

다른건 다 그대로 두고 부팅 디스크만 변경하면 된다.

![](/file/image/kubernetes-tools-02.png)

![](/file/image/kubernetes-tools-03.png)

이렇게 똑같이 3개 더 만든다.

![](/file/image/kubernetes-tools-04.png)

참고로 예제는 VM instance-01에서 02, 03, 04로 접근할 수 있도록 만들 것이다.<br>

우선 VM instance-01의 ssh에 접근해보자.

![](/file/image/kubernetes-tools-05.png)

![](/file/image/kubernetes-tools-06.png)

ssh키를 생성

```s
$ ssh-keygen -t rsa
```

잘 생성되었는지 확인

```s
$ ls -al .ssh/
```

![](/file/image/kubernetes-tools-07.png)

파일이 보인다면 생성완료<br>

공개키의 내용을 복사해둔다.

```s
$ cat .ssh/id_rsa.pub
```

위에 나온 내용을 별도로 복사해 둔다.

![](/file/image/kubernetes-tools-08.png)

ssh를 넣는다

Kubespray 설치(instance-01의 ssh에서)

```s
$ ssh intance-2 hostname
$ ssh intance-3 hostname
$ ssh intance-4 hostname
$ ssh intance-5 hostname
# 모두 yes하면된다.
```

```s
$ sudo apt update
$ sudo apt -y install python-pip
```

```s
$ pip --version
```

![](/file/image/kubernetes-tools-09.png)

```s
$ git clone https://github.com/kubernetes-sign/kubespray.git
$ cd kubespray
$ git checkout -b v2.11.0
$ sudo pip install -r requirements.txt
$ ansible --version
```

![](/file/image/kubernetes-tools-10.png)

설치완료!<br>
kubespray 설정

```s
 ~/kubespray $ cp -rfp inventory/sample inventory/mycluster
 ~/kubespray $ ls inventory/mycluster
```

![](/file/image/kubernetes-tools-11.png)

```s
~/kubespray $ vi inventory/mycluster/inventory.ini
```

![](/file/image/kubernetes-tools-12.png)

아래와 같이 수정한다.

```
# ## Configure 'ip' variable to bind kubernetes services on a
# ## different ip than the default iface
# ## We should set etcd_member_name for etcd cluster. The node that is not a etcd member do not need to set the valu
e, or can set the empty string value.
[all]
instance-1 ansible_ssh_host=10.128.0.2 ip=10.128.0.2 etcd_member_name=etcd1
instance-2 ansible_ssh_host=10.128.0.3 ip=10.128.0.3 etcd_member_name=etcd2
instance-3 ansible_ssh_host=10.128.0.4 ip=10.128.0.4 etcd_member_name=etcd3
instance-4 ansible_ssh_host=10.128.0.5 ip=10.128.0.5
instance-5 ansible_ssh_host=10.128.0.6 ip=10.128.0.6

[kube-master]
instance-1
instance-2
instance-3

[etcd]
instance-1
instance-2
instance-3

[kube-node]
instance-4
instance-5

[calico-rr]

[k8s-cluster:children]
kube-master
kube-node
calico-rr
```

마지막으로 클러스터를 구성하는 명령을 실행

```s
~/kubespray $ ansible-playbook -i inventory/mycluster/inventory.ini -v --become --become-user=root cluster.yml
```

![](/file/image/kubernetes-tools-13.png)

동작을 확인

```s
$ sudo -i
$ kubectl get node
```

![](/file/image/kubernetes-tools-14.png)