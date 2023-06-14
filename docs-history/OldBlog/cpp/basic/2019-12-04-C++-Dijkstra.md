---
title: "(C++) Dijkstra Algorithm"
date: 2019-12-11 00:00:00 -0000
---

```cpp
#include <iostream>
#include <cstdlib>
#include <map>
#include <vector>
using namespace std;

enum Color { WHITE, GREY, BLACK };

struct node
{
  string name;
  Color color;
  map<node*,int> voisins;
  
  node(string aname) : name(aname), color(WHITE){}
};

node* findNode(const vector<node>& v, const string& name)
{
  for(int i=0;i<v.size();i++)
  {
    if(v[i].name==name)
    {
      return (node*)&v[i];
    }
  }
  return NULL;
}

void addConn(vector<node>& nodes, const string& conn)
{
  int dashPos=conn.find('-');
  node* node1=findNode(nodes,conn.substr(0,dashPos));
  node* node2=findNode(nodes,conn.substr(dashPos+1));
  node1->voisins[node2]=1;
  node2->voisins[node1]=1;
}

bool existsGreyNode(const vector<node>& v)
{
  for(int i=0;i<v.size();i++)
  {
    if(v[i].color==GREY)
    {
      return true;
    }
  }
  return false;
}

node* getOptGreyNode(vector<node>& nodes, map<node*,int>& d)
{
  int min=(unsigned int)-1>>1;
  node* optNode=NULL;
  for(int i=0;i<nodes.size();i++)
  {
    if(nodes[i].color==GREY&&d[&nodes[i]]<min)
    {
      min=d[&nodes[i]];
      optNode=&nodes[i];
    }
  }
  return optNode;
}

string dijkstra(vector<node>& nodes, string strStart, string strEnd)
{
  map<node*,int> d;
  map<node*,node*> pi;
  for(int i=0;i<nodes.size();i++)
  {
    d[&nodes[i]]=(unsigned int)-1>>1;       // max integer number
    pi[&nodes[i]]=NULL;
  }
  node* start=findNode(nodes,strStart);
  d[start]=0;
  start->color=GREY;
  while(existsGreyNode(nodes))
  {
    node* optNode=getOptGreyNode(nodes,d);
    for(map<node*,int>::iterator it=optNode->voisins.begin();
        it!=optNode->voisins.end();it++)
    {
      if(it->first->color==WHITE||it->first->color==GREY)
      {
        if(d[it->first]>d[optNode]+it->second)
        {
          d[it->first]=d[optNode]+it->second;
          pi[it->first]=optNode;
        }
        it->first->color=GREY;
      }
    }
    optNode->color=BLACK;
  }
  node* end=findNode(nodes,strEnd);
  if(end->color==WHITE)
  {
    return "-1";
  }
  vector<string> reversePath;
  for(node* n=end;n;n=pi[n])
  {
    reversePath.push_back(n->name);
  }
  string out="";
  for(int i=reversePath.size()-1;i>=0;i--)
  {
    out+=reversePath[i]+'-';
  }
  return out.substr(0,out.length()-1); 
}

string ShortestPath(string strArr[], const int len) { 

  // code goes here   
  int nbNodes=strtol(strArr[0].c_str(),NULL,10);
  vector<node> nodes;
  for(int i=1;i<=nbNodes;i++)
  {
    nodes.push_back(strArr[i]);
  }
  for(int i=nbNodes+1;i<len;i++)
  {
    addConn(nodes,strArr[i]);
  }
  return dijkstra(nodes,strArr[1],strArr[nbNodes]); 
            
}

int main() { 
   
  // keep this function call here
  /* Note: In C++ you first have to initialize an array and set 
     it equal to the stdin to test your code with arrays. 
     To see how to enter arrays as arguments in C++ scroll down */
     
  string A[] = gets(stdin);
  cout << ShortestPath(A,sizeof(A)/sizeof(*A));
  return 0;
    
}
```

> * [Run This Code](https://ideone.com/4HEVVW)