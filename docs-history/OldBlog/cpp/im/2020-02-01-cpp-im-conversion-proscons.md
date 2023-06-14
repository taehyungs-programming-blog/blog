---
title: "(C++) 변환의 장단점"
date: 2020-02-01 00:00:00 -0000
---

```cpp
int main()
{
    FILE* f = fopen("a.txt", "wt");
    fputs("hello", f);

    fclose(f);
}
```

> 위는 C코드인데 C코드의 단점은 메모리 할당을 특정지점에서 하면 해지또한 개발자가 해줘야한다는 점이있다<br>
> 생성자에서 메모리를 할당하고 소멸자에서 메모리를 소멸하게 만들어보고 싶다<br>

```cpp
class OFile
{
    FILE* file;
public:
    OFile(const char* filename, const char* mode = "wt")
    {
        file = fopen(filename, mode);
    }
    ~OFile()
    {
        fclose(file);
    }
};

int main()
{
    OFile f("a.txt");
}
```

> 위와 같은 구현을 RAII(Resource Acqusion Is Initialization)이라 한다.

---

```cpp
int main()
{
    OFile f("a.txt");

    // 아래와 같이 OFile을 쓰고 싶은데 f는 FILE주소가 넘어가야하는데... 어떻게 하지?
    // OFile -> FILE* 암시적변환이 되면 되겠네?
    // f.operator FILE*() 구현
    fputs("hello", f);
    fprintf(f, "n = %d", n);
}
```

```cpp
class OFile
{
    FILE* file;
public:
    OFile(const char* filename, const char* mode = "wt")
    {
        file = fopen(filename, mode);
    }
    ~OFile() { fclose(file); }

    operator FILE*() { return file; }
};
```

> 대표적 예시는 string이다.

```cpp
int main()
{
    String s1 = "hello";
    char s2[10];

    strcpy(s2, s1);
    // String -> const char* 암시적 형변환
}
```

---

## 그럼 단점은 뭘까??

> 변환의 위험성

```cpp
class OFile
{
    FILE* file;
public:
    OFile(const char* filename, const char* mode = "wt")
    {
        file = fopen(filename, mode);
    }
    ~OFile() { fclose(file); }

    operator FILE*() { return file; }
};

void foo(OFile f) {}

int main()
{
    OFile f("a.txt");
    foo(f);             // ok

    foo("hello");       // ok ???
    // const char* -> OFile로 암시적 형변환된다.
    // 의도치 않은 버그를 유발할 수 있다.
}
```

> 막아보자.

```cpp
class OFile
{
    FILE* file;
public:
    // explicit : 인자가 한개인 생성자가 암시적 변환을 허용하는 것을 막는다.
    explicit OFile(const char* filename, const char* mode = "wt")
    {
        file = fopen(filename, mode);
    }
    ~OFile() { fclose(file); }

    operator FILE*() { return file; }
};
```