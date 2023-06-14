---
title: "(C++ Quize) Medium : Min Window Substring"
permalink: cpp/quize/min-window-substring/                # link 직접 지정
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
  - min-window-substring
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

## Q

```
Input: {"ahffaksfajeeubsne", "jefaa"}
Output: aksfaje
```

```
Input: {"aaffhkksemckelloe", "fhea"}
Output: affhkkse
```

---

## A

```cpp
#include <iostream>
#include <string>
using namespace std;

string MinWindowSubstring(string strArr[], int arrLength) {
  
  // code goes here
  
  string N = strArr[0];
  string K = strArr[1];
  strArr[0] = "";
  int start_index = 0;
  int final_start_index = -1;
  int min_substring_length = 2147483647;

  int K_hash[256] = {0};
  int N_hash[256] = {0};
  //Counts chars in K and assigns count to corresponding int to char location
  for (int i = 0; i < K.length(); i++){
    K_hash[K[i]]++;
  }

  int count = 0;
  //Counts chars in N and assigns count to corresponding int to char location
  for (int i = 0; i < N.length(); i++){
    N_hash[N[i]]++;
    //Increment count everytime N Hashtable has a character from K Hashtable
    if (K_hash[N[i]] != 0 && N_hash[N[i]] <= K_hash[N[i]]){
      count++;
    }

  //Check to see if count has reached same number as our lenght of K
    if (count == K.length()){
      // 여기 들어오는 시점부터 N_hash내에 K_hash가 모두 들어가 있다
      while (N_hash[N[start_index]] > K_hash[N[start_index]] ||         // 찾는 문자열을 더 갖고있거나
            K_hash[N[start_index]] == 0){                               // 찾고있는 문자가 아니거나
        if (N_hash[N[start_index]] > K_hash[N[start_index]]){
          N_hash[N[start_index]]--;
        }
        start_index++;                                                  // 인덱스에서 빼버린다
      }
      int substring_lenght = i - start_index + 1;
      if (substring_lenght < min_substring_length){
          min_substring_length = substring_lenght;
          final_start_index = start_index;
      }
    }
  }

  if (final_start_index != -1){
    strArr[0] = N.substr(final_start_index, min_substring_length);
  }

  return strArr[0];

}

int main(void) { 
   
  // keep this function call here
  /* Note: In C++ you first have to initialize an array and set 
     it equal to the stdin to test your code with arrays. */

  string A[] = coderbyteInternalStdinFunction(stdin);
  int arrLength = sizeof(A) / sizeof(*A);
  cout << MinWindowSubstring(A, arrLength);
  return 0;
    
}
```