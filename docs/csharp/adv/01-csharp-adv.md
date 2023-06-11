---
layout: default
title: "1. IL(Intermediate Language)"
parent: (C# Adv)
grand_parent: "(C#)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## IL (Intermediate Language)

* C# <- (IL) -> 가상 머신
* C#언어를 가상 머신이 빌드할 수 있게(읽을 수 있게) 해주는 언어라 생각하면된다.
* 이게 왜 필요한가? .NET은 C++, VB로도 빌드가 가능하기 때문
* IL이 C#을 사용하는데 필수는 아니지만 알아두면 좋다.

---

## IL을 알아두면 좋은점?

```csharp
using System;

struct Point
{
    public int x;
    public int y;
}

class Program
{
    public static void Main()
    {
        Point pt1;                  // 값 타입
        Point pt2 = new Point();    // 참조 타임
        // 값, 참조 타임의 차이점???
        Console.WriteLine($"{pt1.x}");  // Error - 초기화 안됨
        Console.WriteLine($"{pt2.x}");  // Warring
    }
}
```

```bash
$ csc sample.cs # 빌드
$ ildsam sample.exe /out=b.txt
```

```
// ...
.method public hidebysig static void  Main() cil managed
{
.entrypoint
// 코드 크기       10 (0xa)
.maxstack  1
.locals init (valuetype Point V_0,  // V_0 == pt1
            valuetype Point V_1)    // V_1 == pt2
IL_0000:  nop
IL_0001:  ldloca.s   V_1            // V_1의 경우 initobj를 한다.
IL_0003:  initobj    Point          
// 이런식으로 IL문법을 볼 경우 C#이 내부적으로 어떻게 동작하는지 이해할 수 있음.
// ...
```

---

## IL Programming

### 필수로 들어가야하는 부분

```
// (필수3) exe의 assembly를 지정해 줘야한다.
.assembly ex01 { }

.method static void foo() cil managed
{
    // (필수1) Main에 .entrypoint를 지정해 줘야한다.
    .entrypoint

    // (필수2) 함수에 ret를 지정해 줘야한다.
    ret
}
```

### 함수 호출

```
.assembly ex02 { }

.method static void foo() cil managed
{
    .entrypoint

    call  void goo()
    // ldc - stack에 넣어주세요
    // i4 - int 4bytes입니다.
    // 1 번, 2 번
    ldc.i4.1    // push 1
    ldc.i4.2    // push 2
    call void hoo(int32 a, int32 b)
    ret
}

.method static void hoo(int32 a, int32 b) cil managed
{
    ret
}

.method static void goo() cil managed
{
    ret
}
```

### WriteLine써보기

```
.assembly ex03 { }
// 외부 라이브러리 호출
.assembly extern mscorlib {}

.method static void foo() cil managed
{
    .entrypoint
    
    // C# : System.Console.WriteLine()
    // System.Console - System의 Console object를 호출
    // Console::WriteLine - WriteLine함수 호출
    call void [mscorlib]System.Console::WriteLine()
    // C# : System.Console.WriteLine("Hello, IL")
    ldstr "Hello, IL"
    call void [mscorlib]System.Console::WriteLine(class System.String)

    ret
}
```

### 지역변수 사용하기

```
.assembly ex04 { }
.assembly extern mscorlib {}

//    int x = 2;
//    int y = 20;
//    Console.WriteLine("{0}, {1}", x, y);

.method public static void foo() cil managed
{
    .entrypoint
    // local variable 2개 선언
    .locals init(int32 V_0, int32 V_1)
    ldc.i4.2    // push 2
    // stloc - store local variable 0 에 stack값을 넣어라
    stloc.0     // x = 2
    ldc.i4.s 20 // push 20
    stloc.1     // y = 20
    ldstr "{0}, {1}"
    ldloc.0         // x
    box   int32     // WriteLine이 object로 받는데 int32의 데이터를 boxing해달라
    ldloc.1         // y
    box   int32
    call void [mscorlib] System.Console::WriteLine(string, object, object)
    ret
}
```

#### (복습) boxing/unboxing

* [참고 사이트 🌍](https://taehyungs-programming-blog.github.io/blog/docs/csharp/C%23/07-C%23/)

* **boxing** - value type을 reference type으로 가리킬 경우
    * value를 복사해 heap으로 올리며 복사본을 reference가 가리키게 됨
    * ex) `object o = n1; // int n1 = 10;`
* **unboxing** - reference type을 다시 value type으로 가리킬 경우
    * heap에서 다시 stack으로

### 클래스 만들기

```
.assembly ex05 { }
.assembly extern mscorlib {}

// .class를 지정해준다
.class public Program
{
    .method public static void foo() cil managed
    {
        ldstr "foo"
        call void [mscorlib]System.Console::WriteLine(string)
        ret
    }
    .method public static void Main() cil managed
    {
        .entrypoint
        ldstr "Main"
        call void [mscorlib]System.Console::WriteLine(string)
        // 호출은 Program::
        call void Program::foo()
        ret
    }
}
```

### 객체 생성

```
.assembly ex06 { }
.assembly extern mscorlib {}

.class public Program
{
    .method public instance void foo() cil managed
    {
        ldstr "foo"
        call void [mscorlib]System.Console::WriteLine(string)
        ret
    }

    .method public specialname rtspecialname instance void .ctor() cil managed
    {
        // 기반 클래스 생성자 호출
        ldarg.0
        call       instance void [mscorlib]System.Object::.ctor()
        ret
    }
    
    .method public static void Main() cil managed
    {
        .entrypoint
        // Program p = new Program()
        .locals init( class Program V_0 )
        
        newobj  instance void Program::.ctor()
        stloc.0 
        // p.foo()
        //call void Program::foo()
        ldloc.0 
        callvirt instance void Program::foo()
        ret
    }
}
```
