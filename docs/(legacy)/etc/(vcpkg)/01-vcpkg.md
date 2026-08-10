---
layout: default
title: "vcpkg 사용중 주의 사항"
parent: "(vcpkg)"
grand_parent: "Etc 🛠"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* 보통 아래와 같이 integrate 해서 쓸 텐데

```bash
$ vcpkg integrate install
```

* integrate하면 cmake toolchain을 vs에서 갖게된다.
* toolchain을 vcpkg toolchain을 쓰고 싶지 않다면 remove하자.

```bash
$ vcpkg integrate remove
```
