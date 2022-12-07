---
layout: default
title: "04. 연관관계 매핑"
parent: "(JPA BASIC)"
grand_parent: "Spring 🐍"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 배울것

* 방향(Direction): 단방향, 양방향 
* 다중성(Multiplicity): 다대일(N:1), 일대다(1:N), 일대일(1:1), 다대다(N:M) 이해 
* 연관관계의 주인(Owner): 객체 양방향 연관관계는 관리 주인 이 필요

* 모르겠다면 정상이다 배울것이기 때문에!

---


<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/jpa-basic/jpa-basic-4-1.png"/>
</p>

* 우린 하나의 팀에 여러 선수가 뛸수 있다는 것을 알고있다.
* 이걸 구현해 보자면 ...

```java
@Entity
public class Member {
    @Id @GeneratedValue
    private Long id;
    @Column(name = "USERNAME")
    private String name;
    @Column(name = "TEAM_ID")
    private Long teamId;
// …
}
@Entity
public class Team {
    @Id @GeneratedValue
    private Long id;
    private String name;
// …
}

// Is Good?
```

* Team 혹은 Member에서 서로의 Id값을 갖기에 코드로 사용이 불편하다
* 개선해보자

---

## 단방향 연관관계

```java
@Entity
public class Member {
    @Id
    @GeneratedValue
    private Long id;

    @Column(name = "USERNAME")
    private String name;

    private int age;

    // Member는 여럿에서 Team은 하나로 연관관계를 갖습니다.
    @ManyToOne
    // Join할 Column은 TEAM_ID에요
    @JoinColumn(name = "TEAM_ID")
    private Team team;
}
```

```java
//조회 
 Member findMember = em.find(Member.class, member.getId()); 

//참조를 사용해서 연관관계 조회 
Team findTeam = findMember.getTeam();

// 이런식으로 사용이 가능(엄청좋네)
```

---

## 양방향 연관관계

* 이번엔 Team에서 Member를 찾게 해보자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/jpa-basic/jpa-basic-4-2.png"/>
</p>

* 대략 List로 member관리를 하면될꺼 같은데 ..

```java
@Entity
public class Team {
    @Id @GeneratedValue
    private Long id;

    private String name;

    // mappedBy가 뭐지? -> 뒤에서 설명함.
    @OneToMany(mappedBy = "team")
    List<Member> members = new ArrayList<Member>();
// …

}
```

```java
//조회
 Team findTeam = em.find(Team.class, team.getId());
int memberSize = findTeam.getMembers().size(); //역방향 조회

// 이런식의 사용이 가능해 진다
```

---

### 연관관계의 주인과 mappedBy 

* mappedBy = JPA의 멘탈붕괴 난이도 
* mappedBy는 처음에는 이해하기 어렵다. 
* 객체와 테이블간에 연관관계를 맺는 차이를 이해해야 한다

---

* 이렇게 생각해보자 Member에 Team을 변경하려한다
    * 그럼 Member에서 SetTeam을 통해 Team을 수정해야할까
    * Team에 Member List에서 Member를 수정해야할까???

* 이 기준을 잡아주는것이 연관관계의 주인이다.

* 객체의 두 관계중 하나를 연관관계의 주인으로 지정 
    * 연관관계의 주인만이 외래 키를 관리(등록, 수정) 
    * 주인이 아닌쪽은 읽기만 가능 
    * 주인은 mappedBy 속성 사용X 
    * 주인이 아니면 mappedBy 속성으로 주인 지정

---

* 아니 그럼 누구를 주인으로 해야할까?
    * 외래 키가 있는 있는 곳을 주인으로 정해라 
    * 여기서는 Member의 team이 연관관계의 주인
    * 간단히 생각하자면 1:N관계에서 N을 주인으로 하라는 말이다.

---

### 주의사항

```java
Team team = new Team();
team.setName("TeamA");
em.persist(team);

Member member = new Member();
member.setName("member1");
team.getMembers().add(member);

em.persist(member);

// 이러면 안됨.
// Team은  주인이 아니기에 Member에서 Team을 설정해 줘야한다
```

* 순수 객체 상태를 고려해서 항상 양쪽에 값을 설정하자
* 그냥 맘 편하게 양쪽에 다 업데이트한다고 생각하자.

---

## 양방향 매핑 정리 

* 단방향 매핑만으로도 이미 연관관계 매핑은 완료 
* 양방향 매핑은 반대 방향으로 조회(객체 그래프 탐색) 기능이 추 가된 것 뿐 
* JPQL에서 역방향으로 탐색할 일이 많음 
* 단방향 매핑을 잘 하고 양방향은 필요할 때 추가해도 됨 (테이블에 영향을 주지 않음)

---

## Example

* [Clone Code 🌎](https://github.com/EasyCoding-7/ex1-hello-jpa/tree/5)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/jpa-basic/jpa-basic-4-3.png"/>
</p>

```java
@Entity
@Table(name = "ORDERS")
public class Order {
    @Id @GeneratedValue()
    @Column(name = "ORDER_ID")
    private Long id;

    @ManyToOne
    @JoinColumn(name = "MEMBER_ID")
    private Member member;

    @Temporal(value = TemporalType.DATE)
    private Date orderDate;

    @Enumerated(EnumType.STRING)
    private OrderStatus status;
```

```java
@Entity
public class OrderItem {
    @Id @GeneratedValue()
    @Column(name = "ORDER_ITEM_ID")
    private Long id;

    @ManyToOne
    @JoinColumn(name = "ORDER_ID")
    private Order order;

    @ManyToOne
    @JoinColumn(name = "ITEM_ID")
    private Item item;
```

* 양방향관계를 주입해보자 (꼭 하지 않아도 됨)

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

    @OneToMany(mappedBy = "member")
    List<Order> orders = new ArrayList<Order>();
```

```java
@Entity
@Table(name = "ORDERS")
public class Order {
    @Id @GeneratedValue()
    @Column(name = "ORDER_ID")
    private Long id;

    @ManyToOne
    @JoinColumn(name = "MEMBER_ID")
    private Member member;

    @Temporal(value = TemporalType.DATE)
    private Date orderDate;

    @Enumerated(EnumType.STRING)
    private OrderStatus status;

    @OneToMany(mappedBy = "order")
    private List<OrderItem> orderItemList = new ArrayList<OrderItem>();
```