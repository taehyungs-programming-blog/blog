---
title: "(C++ Quize) Medium : Tree Constructor Two"
permalink: cpp/quize/tree-constructor-two/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-03-04 00:00:00 -0000
last_modified_at: 2021-03-04 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
  - quize
category:
  - tree-constructor-two
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

## Q

```
Input: {"(1,2)", "(2,4)", "(5,7)", "(7,2)", "(9,5)"}
Output: true
```

```
Input: {"(1,2)", "(1,3)"}
Output: false
```

---

## A

```cpp
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <queue>

using namespace std;

string TreeConstructorTwo(vector<string> strArr) {
  if(strArr.size() == 0)
  {
    return "true";
  }

  map<int, set<int>> children;
  map<int, int> parent;
  //map<int, set<int>> cons;
  set<int> nodes;
  for(auto s: strArr)
  {
    s = s.substr(1, s.size()-2);
    auto it = find(s.begin(), s.end(), ',');
    int d = distance(s.begin(), it);
    int c = stoi(s.substr(0, d));       // c : child
    int p = stoi(s.substr(d+1));        // p : parent
    children[p].insert(c);
    if(parent.find(c) != parent.end())
    {
        // c의 parent가 있는지 확인, 만약 있다면 false
      return "false";
    }
    parent[c] = p;
    nodes.insert(c);
    nodes.insert(p);
  }
  int root = -1;
  for(auto el: nodes)
  {
      // parent가 없는애들 중에서(root를 찾는과정)
    if(parent.find(el) == parent.end())
    {
      if(root != -1)
      {
          // root가 2개 이상인 경우 false
        //cout << "root already present " << root << " " << el << endl;
        return "false";
      }
      else
      {
        root = el;
      }
    }
  }
  if(root == -1)
  {
      // root가 없는경우 false
    //cout << "no root found" << endl;
    return "false";
  }
  for(auto el: children)
  {
      // child가 2개이상인 경우 체크
    if(el.second.size() > 2)
    {
      //cout << el.first << " too many children" << endl;
      return "false";
    }
  }
  set<int> visited;
  queue<int> q;
  q.push(root);
  while(!q.empty())
  {
    int t = q.front();
    q.pop();
    if(visited.find(t) != visited.end())
    {
        // 기존에 확인했던 노드인지 확인
      return "false";
    }
    visited.insert(t);
    for(auto c: children[t])
    {
      q.push(c);
    }
  }
  if(visited != nodes)
  {
    return "false";
  }
  return "true";
}

int main(void) { 
   
  // keep this function call here
  /* Note: In C++ you first have to initialize an array and set 
     it equal to the stdin to test your code with arrays. */

  vector<string> A = gets(stdin);
  cout << TreeConstructorTwo(A);
  return 0;
    
}
```