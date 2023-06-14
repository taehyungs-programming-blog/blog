---
title: "(C++ Quize) string 사용법 정리"
permalink: cpp/quize/how-to-use-string/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-03-14 00:00:00 -0000
last_modified_at: 2021-03-14 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
  - quize
category:
  - how-to-use-string
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

## find_last_of : 뒤에서 부터 글자 찾기

```cpp
#include <iostream>
using namespace std;

int main() {
	string str = "Hello . String";
	size_t pos = str.find_last_of(".");

	cout << pos << endl;        // 6

	return 0;
}
```

* [Go IDE](https://ideone.com/Ssb3GF)

---

## find : 문자열 앞에서 찾기

```cpp
it  fork  download template  copy
#include <iostream>
using namespace std;
 
int main() {
	string str = "3 + 4 = 7";
 
	if(str.find('+') != string::npos)
	{
		cout << str.find('+');      // 2
	}
 
	if(str.find('-') != string::npos)
	{
		cout << str.find('-');
	}
 
	return 0;
}
```

* [Go IDE](https://ideone.com/2inySB)

---

## substr : 문자열 빼기

```cpp
#include <iostream>
using namespace std;
 
int main() {
	string str = "Hello World!";
 
	// H/e/l/l/o/ /W/o/r/l/d /!
	// 0/1/2/3/4/5/6/7/8/9/10/11
 
	string newStr = str.substr(6, 11);      // World!
	cout << newStr << endl;
 
	return 0;
}
```

* [Go IDE](https://ideone.com/2MWETD)

---

## distance : 문자간 거리 찾기

```cpp
#include <iostream>
#include <algorithm>        // for find
#include <string>           // for distance
using namespace std;

int main() {
	string str = "Hello, World";
	auto it = find(str.begin(), str.end(), ',');
	int dis = distance(str.begin(), it);

	cout << dis << endl;        // 5

	return 0;
}
```

* [Go IDE](https://ideone.com/mIOJhj)

---

## 특정 문자 기준으로 파싱하기

```cpp
#include <iostream>
#include <vector>
#include <sstream>		// for istreamstring
using namespace std;

int main() {
	string str = "1, 2, 3, 4, 5";

	vector<int> vec;
	string line;
	istringstream iss(str);
	while(getline(iss, line, ',')) {
		int value;
		istringstream issl(line);
		issl >> value;
		vec.push_back(value);
	}

	for(int i = 0; i < vec.size(); i++)
	{
		cout << vec[i] << endl;
	}
	return 0;
}
```

* [Go IDE](https://ideone.com/EvrlbM)