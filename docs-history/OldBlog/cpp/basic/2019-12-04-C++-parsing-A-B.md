---
title: "(C++) 문자열 파싱 'A-B'라는 문자열에서 A,B파싱"
date: 2019-12-07 00:00:00 -0000
---

```cpp
#include<iostream>

using namespace std;

int main() {
    string s = "A-B";

    int dashPos = s.find('-');
    string A = s.substr(0, dashPos);
    string B = s.substr(dashPos+1);

    cout << "A: " << A << " B: " << B;

    return 0;
}
```

> * [Run This Code](https://ideone.com/SA5lyL)