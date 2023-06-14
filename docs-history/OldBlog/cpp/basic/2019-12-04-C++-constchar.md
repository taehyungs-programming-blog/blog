---
title: "(C++) const char *& VS const char * 차이점"
date: 2019-12-07 00:00:00 -0000
---

```cpp
#include <iostream>
using namespace std;

void Fn2(const char*& ch) {
// void Fn2(const char* ch) {       // 제대로 출력이 안됨
	cout << *ch++ << endl;
}

void Fn(const char* ch) {
	while(ch) {
		Fn2(ch)	;
	}
}

int main() {
	// your code goes here
	const char * ch = "test";
	Fn(ch);
	return 0;
}
```

> * [Run This Code](https://ideone.com/ciLUK4)