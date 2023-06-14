---
title: "(C++) move와 const object"
date: 2020-02-12 00:00:00 -0000
---

```cpp
int main()
{
  Test t1;
  Test t2 = move(t1);
  
  // 만약 t1이 상수라도 move가 동작할까??
}
```

```cpp
const Test t1;
Test t2 = move(t1);   // copy로 동작!, 모든 멤버를 변경할 수 없기에, 그런데 어떻게 copy가 호출되나?
```

```cpp
Test t2 = static_cast<Test&&>(t1);    // error, 이런 방법은 아니군...
```

```cpp
template<typename T> typename 
remove_reference<T>::type && xmove(T&& obj)
{
  return static_cast<typename remove_reference<T>::type &&>(obj);
  // 결국 return은
  // return static_cast<const Test&&>(obj); 가 되고
  // const Test&&  => Test(Test&&) -> move에서는 호출 불가
  //               => Test(const Test&) -> copy에서 호출 가능
}
```