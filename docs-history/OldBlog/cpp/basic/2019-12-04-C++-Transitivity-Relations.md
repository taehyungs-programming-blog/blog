---
title: "(C++) Transitivity Relations"
date: 2019-12-15 00:00:00 -0000
---

### 문제

Have the function TransitivityRelations(strArr) read the strArr parameter being passed which will make up an NxN matrix where the rows are separated by each pair of parentheses (the matrix will range from 2x2 to 5x5). The matrix represents connections between nodes in a graph where each node corresponds to the Nth element in the matrix (with 0 being the first node). If a connection exists from one node to another, it will be represented by a 1, if not it will be represented by a 0. For example: suppose strArr were a 3x3 matrix with input ["(1,1,1)","(1,0,0)","(0,1,0)"], this means that there is a connection from node 0->0, 0->1, and 0->2. For node 1 the connections are 1->0, and for node 2 the connections are 2->1. This can be interpreted as a connection existing from node X to node Y if there is a 1 in the Xth row and Yth column. Note: a connection from X->Y does not imply a connection from Y->X.

What your program should determine is whether or not the matrix, which represents connections among the nodes, is transitive. A transitive relation means that if the connections 0->1 and 1->2 exist for example, then there must exist the connection 0->2. More generally, if there is a relation xRy and yRz, then xRz should exist within the matrix. If a matrix is completely transitive, return the string transitive. If it isn't, your program should return the connections needed, in the following format, in order for the matrix to be transitive: (N1,N2)-(N3,N4)-(...). So for the example above, your program should return (1,2)-(2,0). You can ignore the reflexive property of nodes in your answers. Return the connections needed in lexicographical order [e.g. (0,1)-(0,4)-(1,4)-(2,3)-(4,1)].

---

### 풀이

간단하게 정리하면...

0 -> 1

1 -> 2

할 때

0 -> 2 를 찾는 것이 문제이다.

`matTr[i][j] = matTr[i][j] || matTr[i][k] && matTr[k][j];` 핵심은 이 코드이고, `matTr[i][j]` 자신이 직접 갈 수 있거나, `matTr[i][k] && matTr[k][j]` k 라는 노드를 통해서 갈 수 있으면 가능으로 체크

그런데 두 번 을 건너 가는 경우 체크가 되는가? 예를들어

0 -> 1

1 -> 2

2 -> 3

일 때

0 -> 3이 가능한지 찾을 수 있는가?

우선

0 -> 1

1 -> 2

이 찾아지기에 0 -> 2로 축소된다. 그럼

0 -> 2

2 -> 3이 되기에 0 -> 3이 가능해진다.(자알 생각해보면 이해가 될 것이다...)

---

```cpp
#include <iostream>
#include <cstring>
#include <sstream>
using namespace std;

string TransitivityRelations(string strArr[], const int size) { 

  // code goes here   
  bool** matAdj = new bool*[size];
  bool** matTr = new bool*[size];
  for(int i=0;i<size;i++)
  {
    matAdj[i]=new bool[size];
    matTr[i]=new bool[size];
    strArr[i].erase(strArr[i].begin());
    strArr[i].erase(strArr[i].end()-1);
    int j=0;
    for(char* pch=strtok((char*)strArr[i].c_str(),",");pch;pch=strtok(NULL,","))
    {
      matAdj[i][j]=!strcmp(pch,"1");
      matTr[i][j]=matAdj[i][j];
      j++;
    }
  }
  for(int k=3;k>-1;k--)
  {
    for(int i=0;i<size;i++)
    {
      for(int j=0;j<size;j++)
      {
        matTr[i][j] = matTr[i][j] || matTr[i][k] && matTr[k][j];
      }
    }
  }
  stringstream ss;
  for(int i=0;i<size;i++)
  {
    for(int j=0;j<size;j++)
    {
      if(i!=j&&matAdj[i][j]!=matTr[i][j])
      {
        ss<<'('<<i<<','<<j<<")-";
      }
    }
  }
  for(int i=0;i<size;i++)
  {
    delete[] matAdj[i];
    delete[] matTr[i];
  }
  delete[] matAdj;
  delete[] matTr;
  string out=ss.str();
  if(out.empty())
  {
    return "transitive";
  }
  else
  {
    out.erase(out.end()-1);
    return out; 
  }
            
}

int main() { 
   
  // keep this function call here
  /* Note: In C++ you first have to initialize an array and set 
     it equal to the stdin to test your code with arrays. 
     To see how to enter arrays as arguments in C++ scroll down */
     
  string A[] = gets(stdin);
  cout << TransitivityRelations(A,sizeof(A)/sizeof(*A));
  return 0;
    
}
```