---
layout: default
title: "01. Project Setting + Hello JPA"
parent: "(JPA BASIC)"
grand_parent: "Spring 🐍"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Clone Code 🌎](https://github.com/EasyCoding-7/ex1-hello-jpa/tree/1)

## h2 준비

* 버전을 알고있어야함. 
* 모를 경우

```sql
SELECT H2VERSION() FROM DUAL;
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/jpa-basic/jpa-basic-1-1.png"/>
</p>

---

## Project 생성

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/jpa-basic/jpa-basic-1-2.png"/>
</p>

* maven (pom.xml) 수정

```xml
<project xmlns="http://maven.apache.org/POM/4.0.0" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
  xsi:schemaLocation="http://maven.apache.org/POM/4.0.0 http://maven.apache.org/xsd/maven-4.0.0.xsd">
  <modelVersion>4.0.0</modelVersion>

  <groupId>jpa-basic</groupId>
  <artifactId>ex1-hello-jpa</artifactId>
  <version>1.0-SNAPSHOT</version>
  <packaging>jar</packaging>

  <name>ex1-hello-jpa</name>
  <url>http://maven.apache.org</url>

  <properties>
    <project.build.sourceEncoding>UTF-8</project.build.sourceEncoding>
  </properties>

  <dependencies>
    <dependency>
      <groupId>junit</groupId>
      <artifactId>junit</artifactId>
      <version>3.8.1</version>
      <scope>test</scope>
    </dependency>

    <!-- JPA 하이버네이트 -->
    <dependency>
      <groupId>org.hibernate</groupId>
      <artifactId>hibernate-entitymanager</artifactId>
      <!-- 하이버네이트 버전 찾는법은 아래 참고 -->
      <version>5.6.14.Final</version>
    </dependency>

    <!-- H2 데이터베이스 -->
    <dependency>
      <groupId>com.h2database</groupId>
      <artifactId>h2</artifactId>
      <!-- h2는 위에서 얻은 버전 사용 -->
      <version>2.1.214</version>
    </dependency>

  </dependencies>
</project>

```

### 자신에게 맞는 하이버네이트 버전찾기

* https://spring.io/projects/spring-boot
* Spring Home Page -> Projects -> Learn 클릭

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/jpa-basic/jpa-basic-1-3.png"/>
</p>

* 자신이 사용하고자 하는 Spring Boot의 Reference Doc.을 클릭한다.
* Single Page HTML로 만들고 org.hibernate를 검색 내리다 보면 어떤버전의 hibernate를 지원해주는지 나와있음

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/jpa-basic/jpa-basic-1-4.png"/>
</p>

### persistence.xml 생성

* src/main/resources/META-INF/persistence.xml

```xml
<?xml version="1.0" encoding="UTF-8"?>
<persistence version="2.2"
             xmlns="http://xmlns.jcp.org/xml/ns/persistence" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
             xsi:schemaLocation="http://xmlns.jcp.org/xml/ns/persistence http://xmlns.jcp.org/xml/ns/persistence/persistence_2_2.xsd">
    <persistence-unit name="hello">
        <properties>
            <!-- 필수 속성 -->
            <property name="javax.persistence.jdbc.driver" value="org.h2.Driver"/>
            <property name="javax.persistence.jdbc.user" value="sa"/>
            <property name="javax.persistence.jdbc.password" value=""/>
            <property name="javax.persistence.jdbc.url" value="jdbc:h2:tcp://localhost/~/test"/>
            <!-- sql을 h2기준의 방언을 쓰겠다고 선언 -->
            <property name="hibernate.dialect" value="org.hibernate.dialect.H2Dialect"/>

            <!-- 옵션 -->
            <!-- (참고) command line 주석에 관련된 사항 -->
            <property name="hibernate.show_sql" value="true"/>
            <property name="hibernate.format_sql" value="true"/>
            <property name="hibernate.use_sql_comments" value="true"/>
            <!--<property name="hibernate.hbm2ddl.auto" value="create" />-->
        </properties>
    </persistence-unit>
</persistence>
```

---

## Hello JPA

* [Clone Code 🌎](https://github.com/EasyCoding-7/ex1-hello-jpa/tree/2)

* 대략 JPA는 이렇게 동작한다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/jpa-basic/jpa-basic-1-5.png"/>
</p>

* persistence를 기반으로 EntityManagerfactory를 생성후 Entity를 만들게 된다.
* 뭔 소린지?? 코드로 보자

```java
package hellojpa; 

import javax.persistence.Entity; 
import javax.persistence.Id; 

@Entity 
public class Member { 
    @Id 
    private Long id; 
    private String name; 
    public Long getId() { 
        return id; 
    } 
    public void setId(Long id) { 
        this.id = id; 
    } 
    public String getName() { 
        return name; 
    } 
    public void setName(String name) { 
        this.name = name; 
    } 
}
```

* 위 Member를 기반으로 CRUD를 해보자

```java
public class JpaMain {
    public static void main(String[] args) {
        // persistence.xml의 persistence-unit name을 넣자
        EntityManagerFactory emf = Persistence.createEntityManagerFactory("hello");

        EntityManager em = emf.createEntityManager();
        
        // Write Code
        
        em.close();
        emf.close();
    }
}
```

* h2 db에 테이블을 하나 만들자

```sql
create table Member (
    id bigint not null,
    name varchar(255),
    primary key (id)
);
```

### CREATE

```java
public class JpaMain {
    public static void main(String[] args) {
        EntityManagerFactory emf = Persistence.createEntityManagerFactory("hello");

        EntityManager em = emf.createEntityManager();

        EntityTransaction tx = em.getTransaction();
        tx.begin();

        // Write Code
        Member member = new Member();
        member.setId(1L);
        member.setName("Hello");
        em.persist(member);
        tx.commit();

        em.close();
        emf.close();
    }
}
```

```java
// 코드정리
public class JpaMain {
    public static void main(String[] args) {
        EntityManagerFactory emf = Persistence.createEntityManagerFactory("hello");
        EntityManager em = emf.createEntityManager();

        EntityTransaction tx = em.getTransaction();
        tx.begin();

        try{
            Member member = new Member();
            member.setId(2L);
            member.setName("Hello2");
            em.persist(member);
            tx.commit();
        } catch (Exception e) {
            tx.rollback();
        } finally {
            em.close();
        }
        
        emf.close();
    }
}
```

---

```java
public class JpaMain {
    public static void main(String[] args) {
        EntityManagerFactory emf = Persistence.createEntityManagerFactory("hello");
        EntityManager em = emf.createEntityManager();

        EntityTransaction tx = em.getTransaction();
        tx.begin();

        try{
            Member findMember = em.find(Member.class, 1L);
            findMember.setName("NewName");
            // em.remove(findMember);
            tx.commit();
        } catch (Exception e) {
            tx.rollback();
        } finally {
            em.close();
        }

        emf.close();
    }
}
```

---

* 주의사항
    * 엔티티 매니저 팩토리는 하나만 생성해서 애플리케이션 전체에서 공유 
    * 엔티티 매니저는 쓰레드간에 공유X (사용하고 버려야 한다). 
    * JPA의 모든 데이터 변경은 트랜잭션 안에서 실행

---

## JPQL

* [Clone Code 🌎](https://github.com/EasyCoding-7/ex1-hello-jpa/tree/3)

* JPA는 SQL을 추상화한 JPQL이라는 객체 지향 쿼리 언어 제공 
* SQL과 문법 유사, SELECT, FROM, WHERE, GROUP BY, HAVING, JOIN 지원 
* JPQL은 엔티티 객체를 대상으로 쿼리 
* SQL은 데이터베이스 테이블을 대상으로 쿼리

```java
public class JpaMain {
    public static void main(String[] args) {
        EntityManagerFactory emf = Persistence.createEntityManagerFactory("hello");
        EntityManager em = emf.createEntityManager();

        EntityTransaction tx = em.getTransaction();
        tx.begin();

        try{
            List<Member> results = em.createQuery("select m from Member", Member.class)
                    .getResultList();
            tx.commit();
        } catch (Exception e) {
            tx.rollback();
        } finally {
            em.close();
        }

        emf.close();
    }
}
```

```java
// 이런식으로 커스텀도 가능하다
List<Member> results = em.createQuery("select m from Member", Member.class)
        .setFirstResult(5)
        .setMaxResults(8)
        .getResultList();
```