---
title: "(C++) 문자열 파싱 (A-B)(C-D)라는 문자열에서 A,B,C,D파싱"
date: 2019-12-07 00:00:00 -0000
---

```cpp
#include<iostream>

using namespace std;

int main() {
    string s = "(A-B)(C-D)";

    s.erase(s.begin());
    s.erase(s.end()-1);
    string str1 = s.substr(0, s.find(")("));
    string str2 = s.substr(s.find(")(")+2);

    cout << "A-B: " << str1 << " C-D: " << str2;

    return 0;
}
```

> * [Run This Code](https://ideone.com/bGTQcr)