---
layout: default
title: "[구현] TypeCast"
parent: "(C++) Memory Management"
grand_parent: "Game Server 👾"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get This Code 🌎](https://github.com/EasyCoding-7/Windows_Game_Server_Tutorial/tree/RA-Tag-13)

---

## TypeCast를 왜 써야할까?

```cpp
class Player
{
};

class Knight : public Player
{
};

class Mage : public Player
{
};

Player* p1 = new Knight();
Player* p2 = new Mage();
Knight* k1 = static_cast<Knight*>(p2);  // 이런 실수가 날 수 있다.
// dynamic_cast쓰면 안되나? -> 성능상의 문제로 static_cast를 써야만 한다고 가정하자.
```

😺 자신의 Type을 내부적으로 관리해보면?

```cpp
enum {
    KNIGHT,
    MAGE,
};

class Player
{
    int _type;
};

class Knight : public Player
{
    Knight() { _type = KNIGHT; }
};

class Mage : public Player
{
    Mage() { _type = MAGE; }
};

Player* p1 = new Knight();
Player* p2 = new Mage();

if(p1->_type == KNIGHT)
{
    Knight* k1 = static_cast<Knight*>(p1);
}
```

* **이 방식의 문제점?**
    * 그런데 클래스가 100개가 되면 enum도 100개로 늘려야하나?
    * 그러다 1000, 10000, 1000000개가되면???

😺 이 문제를 해결하기 위해 TypeCast를 사용한다.

---

## 해결책

😺 하나하나 들어가보자 template을 이용해 type을 지정할 예정인데 우선 아래와 같이 type을 지정하게 만들었다.

```cpp
#pragma once
#include "Types.h"

#pragma region TypeList     // TypeList와 관련된 부분

template<typename... T>
struct TypeList;

template<typename T, typename U>
struct TypeList<T, U>
{
    using Head = T;
    using Tail = U;
};

template<typename T, typename... U>
struct TypeList<T, U...>
{
    using Head = T;
    using Tail = TypeList<U...>;
};

#pragma endregion

/*
// 2개 중 하나를 선택한다면
TypeList<Mage, Knight>::Head whoAMI;        // Mage가 선택됨
TypeList<Mage, Knight>::Tail whoAMI2;       // Knight가 선택됨
*/
/*
TypeList<Mage, TypeList<Knight, Archer>>::Head whoAMI;              // Mage
TypeList<Mage, TypeList<Knight, Archer>>::Tail::Head whoAMI2;       // Knight
TypeList<Mage, TypeList<Knight, Archer>>::Tail::Tail whoAMI3;       // Archer
*/
```

😺 사실 아직까지는 사용성이 좋지 못하고 오히려 더 헷갈린다<br>
😺 우선 type의 길이가 몇 개인지 template으로 출력이 가능할까?<br>

```cpp
// Length 기능 추가

#pragma region Length

template<typename T>
struct Length;

template<>
struct Length<TypeList<>>
{
    enum { value = 0 };
};

template<typename T, typename... U>
struct Length<TypeList<T, U...>>
{
    enum { value = 1 + Length<TypeList<U...>>::value };
};

#pragma endregion

/*
// 몇개의 type이 정의되어 있는지 확인이 가능하다
int32 len1 = Length<TypeList<Mage, Knight>>::value;
*/
```

😺 index를 통해서 type을 지정해 보자

```cpp
#pragma region TypeAt
template<typename TL, int32 index>
struct TypeAt;

template<typename Head, typename... Tail>
struct TypeAt<TypeList<Head, Tail...>, 0>
{
    // index가 0인 경우 Head리턴
    using Result = Head;
};

template<typename Head, typename... Tail, int32 index>
struct TypeAt<TypeList<Head, Tail...>, index>
{
    // 1개 씩 줄여가며 Result를 찾는다
    using Result = typename TypeAt<TypeList<Tail...>, index - 1>::Result;
};

#pragma endregion
/*
using TL = TypeList<Mage, Knight, Archer>
TypeAt<TL, 0>::Result whoAMI;       // Mage
TypeAt<TL, 1>::Result whoAMI;       // Knight
TypeAt<TL, 2>::Result whoAMI;       // Archer
*/
```

😺 반대로 type이 몇 번째 index인지 확인도 가능해야 한다.

```cpp
#pragma  region IndexOf
template<typename TL, typename T>
struct IndexOf;

template<typename... Tail, typename T>
struct IndexOf<TypeList<T, Tail...>, T>
{
    enum { value = 0 };
};

template<typename T>
struct IndexOf<TypeList<>, T>
{
    // 빈타입
    enum { value = -1 };
};

template<typename Head, typename... Tail, typename T>
struct IndexOf<TypeList<Head, Tail...>, T>
{
private:
    enum { temp = IndexOf<TypeList<Tail...>, T>::value };
public:
    enum { value = (temp == -1) ? -1 : temp + 1 };
};

#pragma endregion
/*
int32 index = IndexOf<TL, Mage>::value;     // 1
*/
```

😺 또한 서로 형 변환이 가능한지 확인이 가능해야 한다.

```cpp
#pragma region Conversion

// From에서 To로 변환이 가능한지 확인
template<typename From, typename To>
class Conversion
{
private:
    using Small = __int8;
    using Big = __int32;
    // 컴파일러가 From을 To로 변환을 시도해보고 안되면 -> static Big Test(...) 로 간다
    static Small Test(const To&) { return 0; }  // Test(...)보다 호출의 우선순위가 높다
    static Big Test(...) { return 0; }
    static From MakeFrom() { return 0; }

public:
    enum
    {
        exists = sizeof(Test(MakeFrom())) == sizeof(Small)
        // Test(MakeFrom()) == Test(From) -> From을 const To& 혹은 ...으로 변환이 가능한지
    };
};

#pragma endregion
/*
bool canConver1 = Conversion<Player, Knight>::exists;
bool canConver2 = Conversion<Knight, Player>::exists;
*/
```

```cpp
#pragma region TypeCast

template<int32 v>
struct Int2Type
{
    enum { value = v };
};

template<typename TL>
class TypeConversion
{
public:
    enum
    {
        length = Length<TL>::value
    };

    TypeConversion()
    {
        MakeTable(Int2Type<0>(), Int2Type<0>());
    }

    template<int32 i, int32 j>
    static void MakeTable(Int2Type<i>, Int2Type<j>)
    {
        using FromType = typename TypeAt<TL, i>::Result;
        using ToType = typename TypeAt<TL, j>::Result;
        if (Conversion<const FromType*, const ToType*>::exists)
            s_convert[i][j] = true;
        else
            s_convert[i][j] = false;
        MakeTable(Int2Type<i>(), Int2Type<j + 1>());
    }

    template<int32 i>
    static void MakeTable(Int2Type<i>, Int2Type<length>)
    {
        MakeTable(Int2Type<i + 1>(), Int2Type<0>());
    }

    template<int j>
    static void MakeTable(Int2Type<length>, Int2Type<j>)
    {
    }

    static inline bool CanConvert(int32 from, int32 to)
    {
        static TypeConversion conversion;
        return s_convert[from][to];
    }

public:
    static bool s_convert[length][length];
};

template<typename TL>
bool TypeConversion<TL>::s_convert[length][length];

template<typename To, typename From>
To TypeCast(From* ptr)
{
    if (ptr == nullptr)
        return nullptr;
    using TL = typename From::TL;
    if (TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value))
        return static_cast<To>(ptr);
    return nullptr;
}

template<typename To, typename From>
shared_ptr<To> TypeCast(shared_ptr<From> ptr)
{
    if (ptr == nullptr)
        return nullptr;
    using TL = typename From::TL;
    if (TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value))
        return static_pointer_cast<To>(ptr);
    return nullptr;
}

template<typename To, typename From>
bool CanCast(From* ptr)
{
    if (ptr == nullptr)
        return false;
    using TL = typename From::TL;
    return TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value);
}

template<typename To, typename From>
bool CanCast(shared_ptr<From> ptr)
{
    if (ptr == nullptr)
        return false;
    using TL = typename From::TL;
    return TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value);
}

#pragma endregion

#define DECLARE_TL      using TL = TL; int32 _typeId;
#define INIT_TL(Type)   _typeId = IndexOf<TL, Type>::value;
```

😺 실사용 예제

```cpp
using TL = TypeList<class Player,class  Mage,class  Knight,class  Archer>;

class Player
{
public:
    Player(){
        INIT_TL(Player);
        //_typeId = IndexOf<TL, Player>::value;
    }

    virtual ~Player() {}

    DECLARE_TL    
    //using TL = TL;
    //int _typeId;
};

class Knight : public Player
{
    Knight() { INIT_TL(Knight); }
};

class Mage : public Player
{
    Mage() { INIT_TL(Mage); }
};

Player* player = new Player();
bool canCast = CanCast<Knight*>(player);    // false
Knight k1 = TypeCast<Knight*>(player);      // nullptr
```