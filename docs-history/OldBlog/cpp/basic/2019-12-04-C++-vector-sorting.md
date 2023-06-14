---
title: "(C++) vector sorting"
date: 2019-12-09 00:00:00 -0000
---

```cpp
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main()
{
    vector<int> v;

    for(int i = 9; i >= 0; i--) {
        v.push_back(i);
    }

    sort(v.begin(), v.end());

    for(int i = 0; i <v.size(); i++) {
        cout << v[i] << endl;
    }

    return 0;
}
```

> * [Run This Code](https://ideone.com/ajiayB)