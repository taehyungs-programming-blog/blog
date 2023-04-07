---
layout: default
title: "03. [JAVA8] stream"
parent: "(JAVA)"
grand_parent: "Spring 🐍"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```java
List<String> name = new ArrayList<>();
name.add("h1");
name.add("h2");
name.add("h3");
name.add("h4");
name.add("h5");


// stream()을 통해 Stream이라는 object로 변경한다.
Stream<String> stringStream = name.stream().map(String::toUpperCase);


// 기존 데이터에는 영향을 주지 않는다.
name.forEach(System.out::println);


// stream을 사용했다해서 바로 적용되진 않는다(lazy하다 라고함)
List<String> collect = name.stream()
        .map((s) -> {
            System.out.println(s);
            return s.toUpperCase();
        })
        .collect(Collectors.toList());  // collect를 호출하지 않는다면 List내부도 호출되지 않음
System.out.println("========================");


// 병렬적 처리도 가능하다.
    // 주의할 점은 병렬처리는 멀티쓰레드를 이용하는데
    // 멀티쓰레드가 무조건 유리하진 않다(데이터가 클 경우만 이용)
List<String> collect2 = name.parallelStream()
        .map(String::toUpperCase)
        .collect(Collectors.toList());
collect2.forEach(System.out::println);
```

---

## stream api example

```java
public class OnlineClass {
    private Integer id;
    private String title;
    private boolean closed;

    public OnlineClass(Integer id, String title, boolean closed) {
        this.id = id;
        this.title = title;
        this.closed = closed;
    }

    public Integer getId() {
        return id;
    }

    public void setId(Integer id) {
        this.id = id;
    }

    public String getTitle() {
        return title;
    }

    public void setTitle(String title) {
        this.title = title;
    }

    public boolean isClosed() {
        return closed;
    }

    public void setClosed(boolean closed) {
        this.closed = closed;
    }
}
```

```java
List<OnlineClass> springClasses = new ArrayList<>();
springClasses.add(new OnlineClass(1, "spring boot", true));
springClasses.add(new OnlineClass(2, "spring data jap", true));
springClasses.add(new OnlineClass(3, "spring mvc", true));
springClasses.add(new OnlineClass(4, "spring core", false));
springClasses.add(new OnlineClass(5, "spring api development", false));


// spring으로 시작되는 수업
springClasses.stream()
        .filter(oc -> oc.getTitle().startsWith("spring"))
        .forEach(oc -> System.out.println(oc.getId()));


// close 되지 않은 수업
springClasses.stream()
        //.filter(oc -> !oc.isClosed())
        .filter(Predicate.not(OnlineClass::isClosed))
        .forEach(oc -> System.out.println(oc.getId()));


// 수업 이름만 별도로 모으기
springClasses.stream()
        .map(oc -> oc.getTitle())
        .forEach(System.out::println);
```

```java
List<OnlineClass> javaClasses = new ArrayList<>();
javaClasses.add(new OnlineClass(6, "The Java Test", true));
javaClasses.add(new OnlineClass(7, "The Java Code", true));
javaClasses.add(new OnlineClass(8, "The Java 8", false));

List<List<OnlineClass>> eventClasses = new ArrayList<>();
eventClasses.add(springClasses);
eventClasses.add(javaClasses);


// 두 수업 목록에 들어있는 모든 수업 아이디 출력
eventClasses.stream()
        .flatMap(Collection::stream)
        .map(oc -> oc.getId())
        .forEach(System.out::println);


// 10부터 1씩 증가하는 무제한 스트림 중에서 앞에 10개 빼고 최대 10개 까지 출력
Stream.iterate(10,i -> i+ 1)
        .skip(10)
        .limit(10)
        .forEach(System.out::println);


// 자바 수업 중에 Test가 들어있는 수업 확인
boolean t = javaClasses.stream()
        .anyMatch(oc -> oc.getTitle().contains("Test"));
System.out.println(t);


// 스프링 수업 중 제목에 spring이 들어간 제목만 모아 List
List<String> c = springClasses.stream()
        .filter(oc -> oc.getTitle().contains("spring"))
        .map(OnlineClass::getTitle)
        .collect(Collectors.toList());
```