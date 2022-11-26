---
layout: default
title: "02. [JAVA8] default interface"
parent: "(JAVA)"
grand_parent: "Spring 🐍"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## default interface

```java
public interface FooInterface {
    // interface는 직접구현해야 한다.
    String printName();

    /**
     * @impleSpec
     * 설명 블라블라
     */
     // default interface는 직접구현하지 않아도 된다.
    default String pringUpperName() {
        System.out.println("hello");
        return "hello";
    }
}
```

---

## JAVA8에서 제공해주는 default method

```java
List<String> name = new ArrayList<>();
name.add("h1");
name.add("h2");
name.add("h3");
name.add("h4");
name.add("h5");


// forEach 
name.forEach(System.out::println);


// spliterator 
Spliterator<String> spliterator = name.spliterator(); 
while(spliterator.tryAdvance(System.out::println)); // tryAdvance - 다음이 있는지 체크 

    // 반으로 쪼개기 
Spliterator<String> spliterator2 = spliterator.trySplit(); 
while(spliterator2.tryAdvance(System.out::println)); 


// stream 
name.stream();  // 요소를 stream으로 만들어준다 
    // Example 
name.stream().map(String::toUpperCase) 
            .filter(s -> s.startsWith("K")) 
            .count(); 


// removeIf 
name.removeIf(s -> s.startsWith("H")); 
name.forEach(System.out::println);
```