---
layout: default
title: "01. [JAVA8] 함수형 인터페이스"
parent: "(JAVA)"
grand_parent: "Spring 🐍"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 함수형 인터페이스 구현

```java
/*
* 함수형 인터페이스 - interface class에 추상 매서드가 하나만 있을경우
 */

@FunctionalInterface
public interface RunSomething {
    abstract void doIt();   // abstract는 생략해도 좋다.
    // void doIt2();        // @FunctionalInterface를 선언했기에 에러가 발생
}
```

```java
public class Foo {
    public static void main(String[] args) {
        RunSomething runSomething = new RunSomething() {
            @Override
            public void doIt() {
                System.out.println("Hello");
            }
        };
        runSomething.doIt();        // 이런식으로 호출하려 쓴다.

        // 람다로 표현시 더 간결하게 사용이 가능하다
        RunSomething runSomthinglambda = () -> System.out.println("Hello");
        runSomthinglambda.doIt();   // 역시 가능
    }
}
```

---

## 자바에서 제공하는 함수형 인터페이스

* 종류가 많기에 찾아 보는것을 추천

```java
import java.util.function.Function;

/*
* 자바에서 제공하는 함수형 인터페이스 Example - Function
 */

public class Plus10 implements Function<Integer, Integer> {
    @Override
    public Integer apply(Integer integer) {
        return integer + 10;
    }
}
```

```java
Function<Integer, Integer> plus10 = (i) -> i + 10;
System.out.println(plus10.apply(10)); // 20

Function<Integer, Integer> multiply2 = (i) -> i * 2;
plus10.compose(multiply2).apply(10);    // 10 * 2 + 10 (compose함수를 먼저 호출함)

plus10.andThen(multiply2).apply(10);    // 10 + 10 * 2 (andThen을 늦게 호출)
```

---

## (TIPS) lambda capture 주의사항

```java
// lambda사용시 주의사항
int baseNumber = 10;

IntConsumer printInt = (i) -> {
    System.out.println(i + baseNumber);
};

printInt.accept(10);

baseNumber++;   // Error! - lamda captor가 가능한 변수는 final한 변수뿐이다.
```

---

## Method Reference

```java
// 아래와 같은 클래스가 있다고 가정하자
public class Greeting {
    
    private String name;

    public Greeting() {
    }

    public Greeting(String name) {
        this.name = name;
    }
    
    public String hello(String name) {
        return "hello " + name;
    }
    
    public static String hi(String name) {
        return "hi " + name;
    }
}
```

```java
// 흠... Greeting의 hi함수와 동일한 기능인데?
UnaryOperator<String> hi = (i) -> "hi " + i;

UnaryOperator<String> hi2 = Greeting::hi;

Greeting greeting = new Greeting("hello");
UnaryOperator<String> hi3 = greeting::hello;

    // object 생성도 가능하다
Supplier<Greeting> newGreeting = Greeting::new;
newGreeting.get();  // Greeting object가 생성된다.

Function<String, Greeting> newGreeting2 = Greeting::new;
Greeting newObject = newGreeting2.apply("hello");

// 추가 Tips
String[] name = {"a", "b", "c"};
Arrays.sort(name, String::compareToIgnoreCase);
```