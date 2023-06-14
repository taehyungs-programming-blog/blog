---
title: "STL : bind/function example"
permalink: cpp/stl/function-example/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-16 00:00:00 -0000
last_modified_at: 2020-04-16 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

* [Run This Code](https://ideone.com/UaSp0l)

```cpp
#include <iostream>
#include <string>
#include <functional>
#include <map>
#include <vector>
using namespace std;
using namespace std::placeholders;

class NotificationCenter
{
    using HANDLER = function<void(void*)>;
    map<string, vector<HANDLER>> notif_map;
public:
    void Register(string key, HANDLER h)
    {
        notif_map[key].push_back(h);
    }
    void Notify(string key, void* param)
    {
        vector<HANDLER>& v = notif_map[key];

        for(auto f : v)
            f(param);
    }
};

void f1(void* p) { cout << "f1" << endl;}
void f2(void* p, int a, int b) { cout << "f2" << endl;}

int main()
{
    NotificationCenter nc;
    nc.Register("CONNECT_WIFI", &f1);
    nc.Register("CONNECT_WIFI", bind(&f2, _1, 0, 0));

    nc.Notify("CONNECT_WIFI", (void*)0);
}
```

