---
title: "(C++) 초기화 코드의 문제 해결"
date: 2020-02-05 00:00:00 -0000
---

```cpp
// 기존초기화의 문제점.
int main()
{
  // 1. 초기화 방법이 너무 다양함. -> 하나로 통일
  /*
  int n1 = 0;
  int n2(0);
  int ar[3] = { 1, 2, 3 };
  Point p = { 1, 2 };     // 구조체
  complex c(1, 2);        // 클래스
  */
  int n1{ 0 };
  int n2{ 0 };
  int ar[3]{ 1, 2, 3 };
  // Point p = { 1, 2 };     // 구조체
  // complex c(1, 2);        // 클래스
    
  // 2. 클래스 내부의 배열을 초기화 할 방법이 없다.
  class Test
  {
    // int x[10];
    int x[10]{ 1, 2, 3 ... , 10 };

  };

  
  // 3. 동적메모리할당을 할 경우 초기화 할 방법이 없다.
  // int* p = new int[3];
  int* p = new int[3]{1,2,3};
  
  // 4. STL 컨테이너를 초기화하는 편리한 방법이 없다.
 vector<int> v{1,2,3};
  /*
  vector<int> v;
  for(int i = 0; i < 10; i++)
    v.push_back(1);
}
*/
```