---
title: "(C++ Quize) Medium : Palindromic Substring"
permalink: cpp/quize/palindromic-substring/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-03-20 00:00:00 -0000
last_modified_at: 2021-03-20 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
  - quize
category:
  - palindromic-substring
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

## Q

```
Input: "hellosannasmith"
Output: sannas
```

```
Input: "abcdefgg"
Output: none
```

## A

```cpp
#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

bool isPalindrome(std::string word) {
  std::string word2 = word;
  std::reverse(word2.begin(), word2.end());
  return word == word2;
}

string PalindromicSubstring(string str) {
  for (int length = str.length(); length > 1 ; --length) {
    for (int i = 0; i < str.length()-length; ++i) {
      std::string word = str.substr(i, length);
      if (isPalindrome(word)) {
        return word;
      }
    }
  }  
  return "none";

}

int main(void) { 
   
  // keep this function call here
  cout << PalindromicSubstring(coderbyteInternalStdinFunction(stdin));
  return 0;
    
}
```