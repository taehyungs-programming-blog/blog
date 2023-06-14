---
title: "(C++ Quize) Medium : Character Removal"
permalink: cpp/quize/character-removal/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-03-18 00:00:00 -0000
last_modified_at: 2021-03-18 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
  - quize
category:
  - character-removal
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

## Q

```
Input: {"baseball", "a,all,b,ball,bas,base,cat,code,d,e,quit,z"}
Output: 4
```

```
Input: {"apbpleeeef", "a,ab,abc,abcg,b,c,dog,e,efd,zzzz"}
Output: 8
```

---

## A

```cpp
#include <iostream>
#include <string>
#include <vector>

using namespace std;

bool is_alphabet(const char &ch) {
    return ((ch >= 'a') && (ch <= 'z'));
}

vector<string> get_words(const string &str) {
    vector<string> result;
    result.clear();
    int i = 0;
    while (i < str.length()) {
        if (!is_alphabet(str[i])) {
            ++i;
            continue;
        }
        string sub = "";
        for (int j = i; j < str.length(); ++j) {
            if (is_alphabet(str[j])) {
                sub += str[j];
            } else {
                break;
            }
        }
        i += sub.length();
        result.push_back(sub);
    }
    return result;
}

int CharacterRemoval(string strArr[]) { 
    vector<string> words = get_words(strArr[1]);
    const int INF = 1e9;
    int best = INF;
    for (int i = 0; i < words.size(); ++i) {
        int u = 0;
        int v = 0;
        while ((u < strArr[0].length()) && (v < words[i].length())) {
            if (strArr[0][u] == words[i][v]) {
                ++u;
                ++v;
                continue;
            }
            ++u;
        }
        if (v == words[i].length()) {
            best = min(best, (int)(strArr[0].length()) - (int)(words[i].length()));
        }
    }
    if (best == INF) {
        return -1;
    }
    return best;
}

int main() { 
   
  // keep this function call here
  /* Note: In C++ you first have to initialize an array and set 
     it equal to the stdin to test your code with arrays. */
     
  string A[] = gets(stdin);
  cout << CharacterRemoval(A);
  return 0;
    
}
```