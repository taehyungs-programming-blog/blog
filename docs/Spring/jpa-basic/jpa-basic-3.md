---
layout: default
title: "03. Entity Mapping"
parent: "(JPA BASIC)"
grand_parent: "Spring 🐍"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Clone Code 🌎](https://github.com/EasyCoding-7/ex1-hello-jpa/tree/4)

## 엔티티 매핑 소개 

* 객체와 테이블 매핑: @Entity, @Table 

* 주의 
    * 기본 생성자 필수(파라미터가 없는 public 또는 protected 생성자) 
    * final 클래스, enum, interface, inner 클래스 사용X 
    * 저장할 필드에 final 사용 X

* 필드와 컬럼 매핑: @Column 
* 기본 키 매핑: @Id 
* 연관관계 매핑: @ManyToOne,@JoinColumn

---

## 데이터베이스 스키마 자동 생성

* DDL(Data Definition Language)을 애플리케이션 실행 시점에 자동 생성
* (애플리케이션 실행 시점에 entity 기반의 table을 생성할 수 있다는 말)

```xml
        <!-- persistence.xml -->


            <!-- ddl.auto를 create로 해보자. -->
            <property name="hibernate.hbm2ddl.auto" value="create" />


        </properties>
    </persistence-unit>
</persistence>
```

* create - 기존테이블 삭제 후 다시 생성 (DROP + CREATE)
* create-drop - create와 같으나 종료시점에 테이블 DROP 
* update - 변경분만 반영(운영DB에는 사용하면 안됨) 
* validate - 엔티티와 테이블이 정상 매핑되었는지만 확인 
* none - 사용하지 않음
* (주의) 운영 장비에는 절대 create, create-drop, update 사용하면 안된다.

```java
@Entity
public class Member {
    @Id
    private Long id;
    
    // 이런 옵션도 넣을 수 있다
    @Column(unique = true, length = 10)
    private String name;
```

---