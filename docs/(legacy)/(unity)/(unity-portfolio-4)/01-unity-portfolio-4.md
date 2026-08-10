---
layout: default
title: "01. 프로젝트 세팅"
parent: "(포트폴리오 4 - 방치형 게임)"
grand_parent: "(Unity ✨)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unity.IncrementalGame.Example/tree/1)

## 필요한 패키지 설치

* Windows -> Package Manager
    * Addressables 설치
    * `+` 누르고 Add Package By Name 선택
    * `com.unity.nuget.newtonsoft-json` 을 입력 후 Add
* 2D Animation을 위해서 Spine Unity준비 [Link 🌎](https://ko.esotericsoftware.com/spine-unity-download)
    * 다운 완료후 실행해 Import하면 된다

---

## .editorconfig 수정

* UTF-8로 자동저장하게 만들어 보자
    * Visual Studio로 열어 Assembly-CSharp 프로젝트 우클릭
    * Add -> Ne EditorConfig (IntelliCode) 선택시, .editorconfig 파일이 생긴다

```
root = true # 기본값은 true해달라

[*]
charset = utf-8
```
