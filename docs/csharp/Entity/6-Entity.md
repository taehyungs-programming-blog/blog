---
layout: default
title: "6. Migration"
parent: (Entity)
grand_parent: "(C#)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Migration 이란?

🦄 상태관리(버전관리)를 Migration이라 생각하면 된다.<br>
🦄 코드의 버전관리는 git으로 하고, **DB의 버전관리**는???<br>
🦄 Migration에는 크게 세 가지 방법이 존재하며 하나 씩 배워볼 예정 **Code-First**, **Database-First**, **SQL-First**

---

## Code-First

🦄 코드를 기준으로 DB를 맞춰준다.<br>
🦄 지금까지 Code-First로 만들고 있었으며, Entity Class, DbContext가 DB의 기준이 된다.

### Add-Migration

🦄 NuGet Package에서 **Microsoft.EntityFrameworkCore.Tools**를 다운 받자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/entity/entity-6-1.png"/>
</p>

```bash
$ Add-Migration [Name]
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/entity/entity-6-2.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/entity/entity-6-3.png"/>
</p>

🦄 만약 Item DB가 조금 수정됐다고 가정해 보자.

```csharp
[Table("Item")]
public class Item
{
    // ...

    // Grade항목이 추가됨
    public int ItemGrade { get; set; }
```

```bash
$ Add-Migration ItemGrade
```

### Migration 적용하기

#### SQL change script

```bash
$ Script-Migration [From] [To] [Options]
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/entity/entity-6-4.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/entity/entity-6-5.png"/>
</p>

🦄 아직은 잘 모르겠으나 sql파일이 생성된다.<br>
🦄 그냥 생성된 sql파일을 db에 적용하면 된다.

```bash
$ Script-Migration ItemGrade HelloMigration
# ItemGrade에서 HelloMigration로 돌려달라
```

#### Database.Migrate 호출

🦄 코드상에서 `db.Database.Migrate()`를 호출하는데 위험해서 잘 사용안됨.

#### Command Line 방식

```bash
$ Update-Database [options]
```

---

## DB-First

🦄 데이터의 기준이 DB가 된다.<br>
🦄 무슨말인고 하니 DB를 분석해 `DbSet<>`을 만든다는 말.
🦄 편리한 테스트를 위해 [EFCorePowerTools 🌍](https://marketplace.visualstudio.com/items?itemName=ErikEJ.EFCorePowerTools) 다운 (오래걸리니 알아두자.)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/entity/entity-6-6.png"/>
</p>

🦄 프로젝트를 우클릭시 **EF Core Power Tools**가 생성됨을 볼 수 있다.<br>
🦄 **Reverse Engineer** 클릭

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/entity/entity-6-7.png"/>
</p>

🦄 **VisualStudio 2022**에서 실행시 실행이 안됨;;

---

## SQL-First

🦄 SQL Script로 정의하겠다.