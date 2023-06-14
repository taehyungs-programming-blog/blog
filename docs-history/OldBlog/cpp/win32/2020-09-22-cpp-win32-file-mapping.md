---
title: "(Win32, MFC) file mapping"
permalink: cpp/win32/file-mapping/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-09-22 00:00:00 -0000
last_modified_at: 2020-09-22 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
category:
  - file-mapping
  - OpenFileMapping
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

* [참고사이트](https://yonghello.tistory.com/entry/%ED%8C%8C%EC%9D%BC%EB%A7%A4%ED%95%91%EC%98%88%EC%A0%9C)

---

## 참고사이트 글자가 잘 안보여서 복사해옴

- 파일 오픈 : `hF=CreateFile("test.txt")`
- 파일 내용을 메모리에 올린다 : `hMapF=CreateFileMapping(hF)`
- 메모리에 올려진 첫번째 주소를 얻는다. : `pF=MapViewOfFile(hMapF)`
- 첫번째 주소로 메모리 내용을 조작한다. : 아래 코드 참조
- 중간중간에 변경된 내용을 강제로 디스크에 쓰게만든다. : `FlushViewOfFile(pF)`

- 해제. : `UnmapViewOfFile(pF);`
- 해제. : `CloseHandle(hMapF);`
- 파일 닫기. : `CloseHandle(hF);`

### <예제>

```cpp
#include <windows.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    HANDLE hF, hMapF;
    DWORD fSize, i;
    char *pF, *pFtmp;

    hF=CreateFile("test.txt",GENERIC_READ|GENERIC_WRITE, 0, NULL, 
                  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    fSize=GetFileSize(hF,NULL);

    hMapF=CreateFileMapping(
                            hF,                          //file handle
                            NULL,
                            PAGE_READWRITE,
                            0,                           //file size
                            0,                           //file size
                            NULL);                    //map name

    if(hMapF==NULL)
    {
        printf("CreateFileMapping() fail");
        CloseHandle(hF);
        return 1;
    }

    pF=MapViewOfFile(hMapF,FILE_MAP_ALL_ACCESS,0,0,0);

    pFtmp=pF;

    for(i=0;i<fSize;i++)
    {
        *pFtmp=(*pFtmp+1);
        pFtmp++;
    }

    //메모리 내용을 강제로 파일에 쓴다.
    FlushViewOfFile(pF, 0);
    UnmapViewOfFile(pF);
    CloseHandle(hMapF);
    CloseHandle(hF);

    return 0;
}
```

---

- Share Memory를 만드는 프로세스 : `CreateFileMapping->MapViewOfFile->UnmapViewOfFile->CloseHandle`
- Share Memory를 Open하는 프로세스 : `OpenFileMapping->MapViewOfFile->UnmapViewOfFile->CloseHandle`

### <예제>

공유메모리를 만들고 공유메모리에 데이터를 써넣는 프로그램

```cpp
#include <windows.h>
#include <stdio.h>

#pragma pack(1)

typedef struct tag_SMTEST
{
    char c;
    int  i;
    short s;
} SMTEST;

int main(int argc, char *argv[])
{
    HANDLE hMapF;
    SMTEST *psm;

    // JJBSMTEST라는 이름으로 공유메모리를 만든다.
    hMapF=CreateFileMapping(
                        INVALID_HANDLE_VALUE,
                        NULL,
                        PAGE_READWRITE,
                        0,    
                        sizeof(SMTEST),    
                        "JJBSMTEST");

    if(hMapF==NULL)
    {
        printf("CreateFileMapping() fail");
        return 1;
    }
    psm=(SMTEST *)MapViewOfFile(hMapF,FILE_MAP_ALL_ACCESS,0,0,sizeof(SMTEST));
    //이후부터 psm 포인터만 읽으면 파일의 내용을 자유롭게 액세스할 수 있다. 
    // 파일이 프로세스의 주소 공간에 맵되어 있으므로 포인터를사용하면 연결된 파일의 원하는 부분을 읽을 수 있다.

    psm->c='a';
    psm->i=100;
    psm->s=10;



    //아래 프로그램을 실행할수있게 시간을 주자
    Sleep(10000);
    UnmapViewOfFile(psm);
    CloseHandle(hMapF);

    return 0;
}
```

### <예제>

공유메모리에 접근하여 데이터를 출력하는 프로그램.

```cpp
#include <windows.h>
#include <stdio.h>

#pragma pack(1)

typedef struct tag_SMTEST
{
    char c;
    int  i;
    short s;
} SMTEST;

int main(int argc, char *argv[])
{
    HANDLE hMapF;
    SMTEST *psm;

    // JJBSMTEST라는 이름으로 공유메모리에 접근한다.
    hMapF=OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "JJBSMTEST");
 
    if(hMapF==NULL)
    {
        printf("OpenFileMapping() fail");
        return 1;
    }
    psm=(SMTEST *)MapViewOfFile(hMapF,FILE_MAP_ALL_ACCESS,0,0,sizeof(SMTEST));

 

    //공유메모리에 설정된 내용을 출력한다.
    printf("c:%c i:%d, s:%d\n",psm->c,psm->i,psm->s);
    UnmapViewOfFile(psm);
    CloseHandle(hMapF);

    return 0;
}
```

### <예제> 파일을 메모리에 올려놓고 마음대로 불러다 쓴다.

```cpp
typedef struct Member
{
    char Id[256];
    char name[256]; 
} Member;

Member * member;
Member mem[1000];

void Init()
{
    int i=0;

    for(i=0; i<1000; i++)
    {
        strcpy(mem[i].Id,   "22");
        strcpy(mem[i].name,   "22");
    }
}


void FileSet()
{
    DWORD dwWritten;

    hFile = CreateFile("c:\\jinyong.dat", GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

    if(hFile!=INVALID_HANDLE_VALUE)
    {
        WriteFile(hFile, mem, sizeof(Member)*1000, &dwWritten, 0);
    }

    CloseHandle(hFile);
}


void Mapping()
{
    HANDLE hFile2;

    int i=0;
    hFile2 = CreateFile("c:\\jinyong.dat", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, 0);
    if(hFile2 != INVALID_HANDLE_VALUE)
    {
        hMap = CreateFileMapping(hFile2, NULL, PAGE_READWRITE, 0, 0, NULL);
        member = (Member *)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    }
    printf("%s\n",member[2].Id);
    printf("%s\n",member[3].name);
}
```
