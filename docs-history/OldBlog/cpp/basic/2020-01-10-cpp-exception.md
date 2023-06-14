---
title: "(C++) exception"
permalink: cpp/exception/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-09-04 00:00:00 -0000
last_modified_at: 2020-09-04 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
category:
  - exception
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

## C방식 오류 처리의 단점

C 방식의 오류 처리 단점

1. 반환 값과 실패를 나타내는 거싱 명확히 분리되어 있지 않음
2. 정상적인 실행흐름과 오류처리의 코드가 분리되어 있지 않음
3. (개발자의 실수로) 오류를 무시할 수 있다.

=> 예외(exception)의 탄생

```cpp
// 기존의 C문법
#include <iostream>

int readFile()
{
    FILE* f = fopen("a.txt", "rt");
    
    if( f == 0 )
        // return -1;
        throw "FileNotFound";
    // ...
    fclose(f);
    return 0;
}

int main()
{
    try
    {
        int ret = readFile();
        // ...
    }
    catch( const char* s )
    {
        std::cout << s << std::endl;    // FileNotFound 출력
    }
    catch( int n )    // 이런식으로 다 잡을 수 있음.
    {
    }
    catch( ... )      
    // 가변인자로 어떠한 인자라도 잡음(제일 위에 있으면 불안스)
    {
    }
    
    /*    // C문법
    if( ret == -1 )
    {
        // 실패
    }
    */
}
```

## 예외와 클래스

예외에 더 많은 정보를 담기 위해서는 클래스를 넘기는게 가장 좋다.<br>
또한 되도록 std::exception으로 상속 받아서 만들면 더 좋다!<br>

```cpp
#include <iostream>

class FileNotFound : public std::exception
{
public:
    virtual const char* what() const noexcept // noexcept 에 대한 설명은 아래에서..
    {
        return "File not Found";
    }
};

void foo()
{
     throw FileNotFound();
}

int main()
{
    try
    {
        foo();
    }
    // catch(FileNotFound e)
    catch(std::exception& e)   // 좀 더 다양하게 받을 수 있게 처리
    // 참조형으로 받는 이유는 그냥 받으면 복사본을 출력하게 됨(주의!!)
    {
        std::cout << e.what() << std::endl;
    }
}
```

---

## noexcept

함수가 예외가 없음(있음)을 표기하는 방법

![](/file/imagecpp-exception-image-01.png)

예외는 왜 표기하나?
 
1. 예외가 없는 함수가 컴파일러 최적화가 더 잘 된다.
2. 예외가 있는지 없는지 조사후에 다른 알고리즘을 사용할 수 있다.

```cpp
void foo()  // 예외가 있다.
void foo() noexcept(false) // 예외가 있다.
void foo() noexcept // 예외가 없다.
{

}

int main()
{
    try
    {
        foo();
    }
    catch( ... )
    {
        
    }
}
```

```cpp
#include <iostream>

void foo()
{
}

int main()
{
    bool b = noexept( foo() );
    
    std::cout << b << std::endl;
}
```

---

## 추가

```cpp
#include <iostream>
using namespace std;

void mightGoWrong() {
    bool error = true;

    if(error) {
        throw 8;
    }
}

int main() {
    mightGoWrong();
    // main 함수에서 error를 아직은 잡을 수 없다.

    return 0;
}
```

```cpp
int main() {
    try {
        mightGoWrong();
    }
    catch(int e){       // throw를 int로 던지기에 이렇게 받을 수 있을 것이다.
        cout << "Error code : " << e << endl;
    }
    cout << "Still running" << endl;
    return 0;
}
```

```cpp
int main() {
    try {
        mightGoWrong();
    }
    catch(int e){
        cout << "Error code : " << e << endl;
    }catch(char const * e) {    // 다른 자료형으로도 받을 수 있다.
        cout << "Error message : " << e << endl;
    }
    cout << "Still running" << endl;
    return 0;
}
```

---

```cpp
class ConGoWrong {
    ConGoWrong() {
        char* pMemory = new char[99999999999999999999]; // 여기서 에러가 발생할 것.
        delete [] pMemory;
    }
};

int main() {
    ConGoWrong wrong;   // 에러를 잡아보자

    return 0;
}
```

```cpp
int main() {
    try{
        ConGoWrong wrong;
    }
    catch(bac_alloc& e){
        cout << e.what() << endl;
    }

    return 0;
}
```

다른 Exception의 정보를 보려면 아래 페이지를 참조하자<br>
https://en.cppreference.com/w/cpp/error/exception

---

## Custom Exception

```cpp
#include <exception>
using namespace std;
// ...
class MyException : public exception
{
public:
    virtual const char * what() const throw() {
        return "Something bad happend!";
    }
};

class Test {
public:
    void goesWrong(){
        throw MyException();
    }
};

int main() {
    Test test;

    try {
        test.goesWrong();
    }
    catch(MyException& e) {
        cout << e.what() << endl;
    }
}
```

---

## Exception Catching Order

```cpp
void goesWrong() {
    bool error1Detected = true;
    bool error2Detected = false;

    if(error1Detected) {
        throw bad_alloc();
    }

    if(error2Detected){
        throw exception();
    }
}

int main() {
    try {
        goesWrong();
    }
    catch(exception& e) {       // 여기서 error1Detected가 잡힌다.
        cout << "exception& e" << e.what() << endl;
    }
    catch(bad_alloc& e) {
        cout << "bad_alloc& e" << e.what() << endl;
    }
    return 0;
}
// bac_alloc을 exception 보다 먼저 검사해야 함.
```
