---
title: "(C++ Quize) Easy : Longest Word"
permalink: cpp/quize/longest-word/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-03-03 00:00:00 -0000
last_modified_at: 2021-03-03 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
  - quize
category:
  - longest-word
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

## Q

```
Input: "fun&!! time"
Output: time
```

```
Input: "I love dogs"
Output: love
```

---

## A

```cpp
#include <iostream>
#include <string>
using namespace std;

string LongestWord(string sen) { 

  // code goes here   
  string longest="";
  string prev_longest="";
  int k=0;
  int last_pos=0;
  for (char& c : sen){
      if (!isalnum(c)){
          longest = sen.substr(last_pos,k-last_pos+1);
          if (prev_longest.length() < longest.length()){
              prev_longest = longest;
          }
          last_pos = k;
      }
      k++;
  }
  longest = sen.substr(last_pos,k-last_pos+1);
  if (prev_longest.length() < longest.length()){
      prev_longest = longest;
  }
  return prev_longest; 
            
}

int main() { 
  
  // keep this function call here
  cout << LongestWord(gets(stdin));
  // cout << LongestWord("test");
  return 0;
    
}
```