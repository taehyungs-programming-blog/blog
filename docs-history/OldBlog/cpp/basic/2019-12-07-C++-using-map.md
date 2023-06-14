---
title: "(C++) map 잘 쓰는 법(vector와 class를 잘 넣고 보는 법)"
date: 2019-12-07 00:00:00 -0000
---

> class와 map을 잘 적용해서 쓰는 방법이다.

```cpp
#include <iostream>
#include <map>
#include <vector>

using namespace std;

struct node {
    string name;
    map<node*,int> amap;

    node(string aname) : name(aname) {}
    // 요런식으로 초기화 하는 팁!
};

// 어떻게 함수에 넣는지 설명하기 위해서 만듦 (굳이 이렇게 안써도 된다는 말)
void addNode(vector<node>& nodes, const string& s) {
    nodes.push_back(s);
}

void addMap(map<node*,int>& maps, vector<node>& nodes) {
    for(int i = 0; i < 5; i++) {
    	maps[&nodes[i]] = (unsigned int)-1>>1;
    	// max interger number	
    }
}

int main() {
    vector<node> nodes;
    map<node*,int> maps;

    for(int i = 0; i < 5; i++) {
        string s = to_string(i);
        addNode(nodes,s);
    }

    for(int i = 0; i < 5; i++) {
        // 간단하게 5개의 오브젝트를 넣어보자.
        addMap(maps, nodes);
    }
    
    for(map<node*,int>::iterator it = maps.begin(); it!=maps.end(); it++) {
    	cout << it->second << endl;
    }

    return 0;
}
```

> * [Run This Code](https://ideone.com/dacMtq)