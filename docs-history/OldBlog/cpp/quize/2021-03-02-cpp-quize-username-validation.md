---
title: "(C++ Quize) Easy : Username Validation"
permalink: cpp/quize/username-validation/                # link 직접 지정
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
  - username validation
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

## Q

1. The username is between 4 and 25 characters.
2. It must start with a letter.
3. It can only contain letters, numbers, and the underscore character.
4. It cannot end with an underscore character.

---

## A

```cpp
#include <iostream>
#include <string>
using namespace std;

string CodelandUsernameValidation(string str) {
  if (str[0] == '_' || str[str.length() - 1] == '_')
    return "false";
  if (str.length() - 1 < 4 || str.length() - 1 > 25)
    return "false";
  for (int i = 0; i < str.length(); i++) {
    if (str[i] < 48 || (str[i] > 57 && str[i] < 65) || 
        (str[i] > 91 && str[i] < 95) || (str[i] > 95 && str[i] < 97) || 
        (str[i] > 122))
        return "false";
  }
  return "true";
}
int main(void) { 
   
  // keep this function call here
  cout << CodelandUsernameValidation(gets(stdin));
  return 0;
    
```