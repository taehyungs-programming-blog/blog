---
title: "(C++) pattern 찾기"
date: 2019-12-07 00:00:00 -0000
---

### Quiz

Have the function PatternChaser(str) take str which will be a string and return the longest pattern within the string. A pattern for this challenge will be defined as: if at least 2 or more adjacent characters within the string repeat at least twice. So for example "aabecaa" contains the pattern aa, on the other hand "abbbaac" doesn't contain any pattern. Your program should return yes/no pattern/null. So if str were "aabejiabkfabed" the output should be yes abe. If str were "123224" the output should return no null. The string may either contain all characters (a through z only), integers, or both. But the parameter will always be a string type. The maximum length for the string being passed in will be 20 characters. If a string for example is "aa2bbbaacbbb" the pattern is "bbb" and not "aa". You must always return the longest pattern possible.

---

```cpp
#include <iostream>
#include <string>

using namespace std;

string PatternChaser(string str) {
    string longestpatt = "";
    for(int pattlen = 2; pattlen <= str.length()/2; pattlen++)
    {
        string currpatt;
        for(int i = 0; i < str.length()-pattlen; i++)
        {
            currpatt = str.substr(i, pattlen);
            if(str.find(currpatt, i+pattlen) != string::npos)
            {
                longestpatt = currpatt;
                break;
            }
        }
    }
    if(longestpatt.empty())
    {
        return "no null";
    }
    return "yes " + longestpatt;
}

int main() {
    cout << PatternChaser(gets(stdin));
    return 0;
}
```

> * [Run This Code](https://ideone.com/7C3fhB)