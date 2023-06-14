---
title: "(C++) GetTickCount"
date: 2020-02-03 00:00:00 -0000
---

```cpp
#include <stdio.h>
#include <windows.h>        // for GetTickCount

int main(int argc, char* argv[]){
	DWORD startTickCount = GetTickCount();
	DWORD currentTickCount;
	while(1) {
		currentTickCount = GetTickCount();
		printf("현재 틱 카운트 : %lu\n", currentTickCount);
		if (currentTickCount - startTickCount >= 1000){
			printf("1 초 지나서 프로그램 종료\n");
			break;
		}
	}
	return 0;
}
```