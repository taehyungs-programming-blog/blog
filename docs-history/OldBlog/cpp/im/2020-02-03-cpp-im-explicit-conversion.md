---
title: "(C++) Explicit Conversion Operator"
date: 2020-02-03 00:00:00 -0000
---

```cpp
class istream
{
public:
  bool fail() { return false; }
  explicit operator boo() { return fail() ? false : true; }
  // C++ 11부터는 변환연산자 앞에 붙일 수 있다.
  // 명시적 변환은 허용되지만 암시적 변환은 될 수 없음.
};
istream cin;

int main()
{
  if( cin ) {}
  bool b1 = cin;    // error
  int n = 10;
  cin << n;         // error
  
  if( cin ) {}      // 이거도 암시적 변환아닌가?
  // C++ 11에서는 if문안에서는 허용
  // 단, if ( cin == false )는 error! - 직접비교는 안된다.
}
```