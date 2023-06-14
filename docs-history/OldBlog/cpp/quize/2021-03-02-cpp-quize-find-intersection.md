---
title: "(C++ Quize) Easy : Find Intersection"
permalink: cpp/quize/find-intersection/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-03-02 00:00:00 -0000
last_modified_at: 2021-03-02 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
  - quize
category:
  - find-intersection
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

## Q

```
Input: {"1, 3, 4, 7, 13", "1, 2, 4, 13, 15"}
Output: 1,4,13
```

```
Input: {"1, 3, 9, 10, 17, 18", "1, 4, 9, 10"}
Output: 1,9,10
```

---

## A

```cpp
#include <iostream>
#include <string>
#include <algorithm>
#include <unordered_set>
#include <sstream>

using namespace std;

vector<int> convertToVector(const string &str) {
  vector<int> result;
  istringstream iss(str);
  string line;
  while (getline(iss, line, ',')) {
    int value;
    istringstream issl(line);
    issl >> value;
    result.push_back(value);
  }
  return result;
}

string FindIntersection(string strArr[]) {
  string result;
  vector<int> vfirst = convertToVector(strArr[0]);
  vector<int> vsecond = convertToVector(strArr[1]);
  int i = 0, j = 0;
  while (i < vfirst.size() && j < vsecond.size()) {
    if (vfirst[i] == vsecond[j]) {
      result += to_string(vfirst[i]);
      result += ",";
      i++; j++;
    }
    else if (vfirst[i] > vsecond[j]) j++;
    else i++;
  }
  if (!result.empty())
    result.pop_back();
  else
    result = "false";
  return result;
}

int main(void) { 
   
  // keep this function call here
  /* Note: In C++ you first have to initialize an array and set 
     it equal to the stdin to test your code with arrays. */

  string A[] = gets(stdin);
  cout << FindIntersection(A);
  return 0;
    
}
```