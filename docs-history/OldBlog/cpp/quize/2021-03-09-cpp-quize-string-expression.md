---
title: "(C++ Quize) Medium : String Expression"
permalink: cpp/quize/string-expression/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-03-09 00:00:00 -0000
last_modified_at: 2021-03-09 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
  - quize
category:
  - string-expression
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

## Q

```
Input: "onezeropluseight"
Output: oneeight
```

```
Input: "oneminusoneone"
Output: negativeonezero
```

---

## A

```cpp
#include <iostream>
#include <string>
using namespace std;

size_t ParseNum(string &str,string num)
{
  size_t pos = str.find(num);
  if(pos!=string::npos)
  {
    int len = num.length();
    string digit;

      if(num== "zero")
        digit = "0";
      if(num== "one")
        digit = "1";        
      if(num== "two")
        digit = "2";        
      if(num== "three")
        digit = "3";        
      if(num== "four")
        digit = "4";        
      if(num== "five")
        digit = "5";        
      if(num== "six")
        digit = "6";      
      if(num== "seven")
        digit = "7";
      if(num== "eight")
        digit = "8";
      if(num== "nine")
        digit = "9";
      if(num== "plus")
        digit = "+";
      if(num== "minus")
        digit = "-";
    str.replace(pos,len,digit);
  }
  return pos;
}

size_t ParseDigit(string &str,string num)
{
  size_t pos = str.find(num);
  if(pos!=string::npos)
  {
    int len = num.length();
    string digit;

      if(num== "0")
        digit = "zero";
      if(num== "1")
        digit = "one";        
      if(num== "2")
        digit = "two";        
      if(num== "3")
        digit = "three";        
      if(num== "4")
        digit = "four";        
      if(num== "5")
        digit = "five";        
      if(num== "6")
        digit = "six";      
      if(num== "7")
        digit = "seven";
      if(num== "8")
        digit = "eight";
      if(num== "9")
        digit = "nine";
      if(num== "-")
        digit = "negative";
    str.replace(pos,len,digit);
  }
  return pos;
}

double eval(string str)
{
  if(str.find('+')==string::npos &&   // + 가 없다면
    str.find('-',1)==string::npos)    // (0번째 -가 존재하는건 빼고 체크) - 가 없다면
  {
    return stod(str); // stod : string to double
  }

  for(int i=1;i<str.length();i++)
  {
    if(str[i]=='-' &&       // - 기호가 붙어있고
      isdigit(str[i-1]))    // 이 전항이 숫자라면
    {
      str.insert(i++,"+");  // + 기호를 넣어라
    }
  }
  
  int plusPos;
  if((plusPos=str.find('+'))!=string::npos)
  {
    return eval(str.substr(0,plusPos))+eval(str.substr(plusPos+1));
  }
  return 0;
}

string StringExpression(string str) {
  
  // code goes here 
  size_t pos; 
  do {pos = ParseNum(str,"zero");}
  while(pos!=string::npos);

  do {pos = ParseNum(str,"one");}
  while(pos!=string::npos);
  
  do {pos = ParseNum(str,"two");}
  while(pos!=string::npos);

  do {pos = ParseNum(str,"three");}
  while(pos!=string::npos);
 
  do {pos = ParseNum(str,"four");}
  while(pos!=string::npos);
 
  do {pos = ParseNum(str,"five");}
  while(pos!=string::npos);

  do {pos = ParseNum(str,"six");}
  while(pos!=string::npos);

  do {pos = ParseNum(str,"seven");}
  while(pos!=string::npos);
 
  do {pos = ParseNum(str,"eight");}
  while(pos!=string::npos);
 
  do {pos = ParseNum(str,"nine");}
  while(pos!=string::npos);

  do {pos = ParseNum(str,"plus");}
  while(pos!=string::npos);

  do {pos = ParseNum(str,"minus");}
  while(pos!=string::npos);

  string result = to_string(int(eval(str)));


  do {pos = ParseDigit(result,"0");}
  while(pos!=string::npos);

  do {pos = ParseDigit(result,"1");}
  while(pos!=string::npos);
  
  do {pos = ParseDigit(result,"2");}
  while(pos!=string::npos);

  do {pos = ParseDigit(result,"3");}
  while(pos!=string::npos);
 
  do {pos = ParseDigit(result,"4");}
  while(pos!=string::npos);
 
  do {pos = ParseDigit(result,"5");}
  while(pos!=string::npos);

  do {pos = ParseDigit(result,"6");}
  while(pos!=string::npos);

  do {pos = ParseDigit(result,"7");}
  while(pos!=string::npos);
 
  do {pos = ParseDigit(result,"8");}
  while(pos!=string::npos);
 
  do {pos = ParseDigit(result,"9");}
  while(pos!=string::npos);

  do {pos = ParseDigit(result,"-");}
  while(pos!=string::npos);

  return result;

}

int main(void) { 
   
  // keep this function call here
  cout << StringExpression(coderbyteInternalStdinFunction(stdin));
  return 0;
    
}
```