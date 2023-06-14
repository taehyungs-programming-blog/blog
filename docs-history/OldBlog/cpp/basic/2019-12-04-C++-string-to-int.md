---
title: "(C++) string to int"
date: 2019-12-10 00:00:00 -0000
---

> 두 가지 방법 중 마음에 드는 방법으로~

```cpp
#include <iostream>
using namespace std;

int main() {
	// your code goes here
	string stra = "10";
	
    // Method 1
	int na1 = strtol(stra.c_str(), NULL, 10);
	cout << na1 << endl;
	
    // Method 2
	int na2 = atoi(stra.c_str());
	cout << na2 << endl;
	
	return 0;
}
```

> * [Run This Code](https://ideone.com/ipHA5g)