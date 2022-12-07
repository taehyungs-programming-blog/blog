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

## 필드, 컬럼 매핑

```java
@Entity
public class Member {
    @Id
    private Long id;

    @Column(name = "name")
    private String username;

    private Integer age;

    @Enumerated(EnumType.STRING)
    private RoleType roleType;

    @Temporal(TemporalType.TIMESTAMP)
    private Date createdDate;

    @Temporal(TemporalType.TIMESTAMP)
    private Date lastModifiedDate;

    @Lob
    private String description;

    // getter, setter
```

```java
public enum RoleType {
    ADMIND, USER
}
```

* @Column 컬럼 매핑
* @Temporal 날짜 타입 매핑 
    * TemporalType.DATE: 날짜, 데이터베이스 date 타입과 매핑 (예: 2013–10–11) 
    * TemporalType.TIME: 시간, 데이터베이스 time 타입과 매핑 (예: 11:11:11) 
    * TemporalType.TIMESTAMP: 날짜와 시간, 데이터베이 스 timestamp 타입과 매핑(예: 2013–10–11 11:11:11)
* @Enumerated enum 타입 매핑 
    * EnumType.ORDINAL: enum 순서를 데이터베이스에 저장
    * EnumType.STRING: enum 이름을 데이터베이스에 저장
    * (주의! ORDINAL 사용X)
* @Lob - 큰 변수, BLOB, CLOB 매핑 
    * CLOB: String, char[], java.sql.CLOB 
    * BLOB: byte[], java.sql. BLOB
* @Transient 특정 필드를 컬럼에 매핑하지 않음(매핑 무시)

* 매핑별로 추가적인 옵션이 있는데 너무많기에 필요할때마다 검색해보는 것을 추천

---

## 기본 키 매핑

### AUTO Generate 전략

* @Id 
* @GeneratedValue

```java
@Id @GeneratedValue(strategy = GenerationType.AUTO)
private Long id;
// (TIPS) Interger보다 미래를 고려해서 .. Long을 쓰자
```

* AUTO Generate 전략 특이한점??

```java
public class JpaMain {
    public static void main(String[] args) {
        EntityManagerFactory emf = Persistence.createEntityManagerFactory("hello");
        EntityManager em = emf.createEntityManager();

        EntityTransaction tx = em.getTransaction();
        tx.begin();

        try{
            Member member = new Member();
            member.setUsername("C");
            
            em.persist(member);
            // persist에서 insert query가 날아간다
            // why? primary key가 자동으로 생성되기에 db에 들어가봐야 pk를 알수 있음
            // 따라서 pk를 구하기 위해서 persist시에 insert query를 날려버린다.
            
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

* 아니 그럼 persist할때마다 insert 쿼리를 날려버리면 네트워크 성능에 너무 안좋지 않나? 
* 그래서 해결법이 있음.

```java
// Exmaple

@Entity
// 잉 sequence전략인디? -> auto, sequence, table이든 사실 큰 상관없음(비슷하게 동작함)
@SequenceGenerator(
        name = “MEMBER_SEQ_GENERATOR",
        sequenceName = “MEMBER_SEQ", 
        initialValue = 1, allocationSize = 50) // 미리 50개의 pk를 만들어주세요
public class Member {
    @Id
    @GeneratedValue(strategy = GenerationType.SEQUENCE,
            generator = "MEMBER_SEQ_GENERATOR")
    private Long id; 
```

### sequence 전략

```java
// Exmaple

@Entity
@SequenceGenerator(
        name = “MEMBER_SEQ_GENERATOR",
        sequenceName = “MEMBER_SEQ", //매핑할 데이터베이스 시퀀스 이름
        initialValue = 1, allocationSize = 1)
public class Member {
    @Id
    @GeneratedValue(strategy = GenerationType.SEQUENCE,
            generator = "MEMBER_SEQ_GENERATOR")
    private Long id; 
```

### table 전략

* 테이블을 만들어 키로 쓴다
    * 그리 잘 사용되진 않음 ...

```sql
create table MY_SEQUENCES (
    sequence_name varchar(255) not null,
    next_val bigint,
    primary key ( sequence_name )
)
```

```java
@Entity
@TableGenerator(
        name = "MEMBER_SEQ_GENERATOR",
        table = "MY_SEQUENCES",
        pkColumnValue = “MEMBER_SEQ", allocationSize = 1)
        public class Member {
        @Id
        @GeneratedValue(strategy = GenerationType.TABLE,
                generator = "MEMBER_SEQ_GENERATOR")
        private Long id; 
```

---

## Example

* [Clone Code 🌎](https://github.com/EasyCoding-7/ex1-hello-jpa/tree/4)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/jpa-basic/jpa-basic-3-1.png"/>
</p>

```java
public class JpaMain {
    public static void main(String[] args) {
        EntityManagerFactory emf = Persistence.createEntityManagerFactory("hello");
        EntityManager em = emf.createEntityManager();

        EntityTransaction tx = em.getTransaction();
        tx.begin();

        try{
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
@Entity
public class Item {

    @Id
    @GeneratedValue()
    @Column(name = "ITEM_ID")
    private Long id;

    private String name;
    private int price;
    private int stockQuantity;
```

```java
@Entity
public class Member {
    @Id @GeneratedValue()
    @Column(name = "MEMBER_ID")
    private Long id;
    private String name;
    private String city;
    private String street;
    private String zipcode;
```

```java
@Entity
@Table(name = "ORDERS")
public class Order {
    @Id
    @GeneratedValue()
    @Column(name = "ORDER_ID")
    private Long id;

    @Column(name = "MEMBER_ID")
    private Long memberId;

    @Temporal(value = TemporalType.DATE)
    private Date orderDate;

    @Enumerated(EnumType.STRING)
    private OrderStatus status;
```

```java
@Entity
public class OrderItem {
    @Id
    @GeneratedValue()
    @Column(name = "ORDER_ITEM_ID")
    private Long id;

    @Column(name = "ORDER_ID")
    private Long orderId;

    @Column(name = "ITEM_ID")
    private Long itemId;

    private int count;
```

```java
public enum OrderStatus {
    ORDER, CANCEL
}

```

* 그런데 ID값으로 다른 Entity를 참고하는게 맞나?
* 너무 사용하기 불편한데?

```java
// 이런식으로 써야한다는 건데 ...
Order order = em.find(Order.class, 1L);
Long memberId = order.getMemberId();
Member member = em.find(Member.class, memberId);
```

* 좀 더 객체지향적으로 설계해보자.