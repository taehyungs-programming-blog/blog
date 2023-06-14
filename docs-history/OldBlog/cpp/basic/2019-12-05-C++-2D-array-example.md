---
title: "(C++) 2차원 string 배열의 문자열을 파싱해서 2차원 int형 배열로 넣기"
date: 2019-12-05 00:00:00 -0000
---

> 입력이 `(1, 2, 3) \r\n (4, 5, 6) ...` 이런식으로 들어온다고 가정한다.
>
> 원하는 데이터는 1 2 3 과 같은 상수만 뽑아내면 된다.

```cpp
#include <iostream>
#include <string.h>

using namespace std;

void Fn(string strArr[], const int size) {
    int** matrix = new int*[size];
    for(int i = 0; i < size; i++) {
        strArr[i].erase(strArr[i].begin());
        strArr[i].erase(strArr[i].end() - 1);
        matrix[i] = new int[size];
        int j = 0;
        for(char* pch = strtok((char*)strArr[i].c_str(),",");pch;pch=strtok(NULL,","))
        {
            matrix[i][j] = strtol(pch,NULL,10);
            // 마지막 10은 진법
            cout << "i: " << i << " j: " << j << " value: " << matrix[i][j] << endl;
            j++;
        }
        for(int i=0;i<size;i++)
        {
            delete[] matrix[i];
        }
        delete[] matrix;
    }
}

int main() {
    string A[] = gets(stdin);
    Fn(A, sizeof(A)/sizeof(*A));
}
```

> * [Run this Code](https://ideone.com/AMhK71)