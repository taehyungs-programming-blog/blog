---
layout: default
title: "[이론/구현] Unicode"
parent: "(C++) 상세 구현"
grand_parent: "Game Server 👾"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get This Code 🌎](https://github.com/EasyCoding-7/Windows_Game_Server_Tutorial/tree/RA-Tag-24)

---

## 이론

👀 문자열(string, wsring)은 어떻게 보낼까?

```cpp
// 우선 들어가기 전 다음의 차이를 이해해 보자.

char snedData[1000] = "가";
char snedData[1000] = u8"가";
WCHAR snedData[1000] = L"가";
TCHAR snedData[1000] = _T("가");

// 가변길이 문자열은 위와같이 다양하게 표기가 가능한데 무슨차일까?
// 서버와 클라는 이런 복잡한 문자열을 어떻게 맞춰야할까?
```

* [참고(UTF-8 Encoding/Decoding) 🌎](https://mothereff.in/utf-8)

### ASCII Code와 Unicode의 등장

👀 처음은 **ASCII Code(문자집합)**로 문자를 표현 `(Ex> A == 0x41(65))`<br>
👀 **문제점?** 서로 다른 문자집합을 사용할 경우 문제가 발생함. (Ex> 영어를 문자집합으로 쓰는 A라는 컴퓨터에서 한글을 문자집합으로 쓰는 B라는 컴퓨터에서 작성된 문서를 읽을경우 깨진 문자로 나타나게 된다.)<br>
👀 **문자집합을 미리 알아야 한다**는 말인데 ... 문자집합 구분이 없는 문자 체계를 써보자. 👉 **Encoding(UNICODE)** 등장

### Encoding의 등장

👀 Unicode라고 장점만 있는 것은 아니다.<br>
👀 영어만 주로 쓰는 환경에서 UNICODE는 보통 2바이트를 쓰는데 이게 과연 효율적인가??<br>
👀 그냥 8 bits인 ASCII를 쓰는게 효율적이지 않나?<br>
👀 그래서 나온게 **ENCODING**! 👉 만약 영어만 쓴다면 UTF-8로 ENCODING을 해줄께!<br>
    Ex) 제일 앞 비트를 0으로 두면 UTF-8이라 가정한다(Ex> 0xxxxxxx)<br>
👀 만약 한글이라면 3바이트를 쓰고(1110xxxx 10xxxxxx 10xxxxxx) 이렇게 들어가 있으면 UTF-8 한글

### UTF-16이란?

👀 그런데 한글은 꽤 널리 쓰이는데 3바이트나 쓰자고?? 👉 **UTF-16**의 탄생<br>
👀 UTF-16은 거의 Unicode를 그대로 쓴다

### 아니 알겠고 ... char는 그럼 뭔데 ASCII냐 UTF냐?

👀 아래 좀 더 자세한 설명이 나오겠지만 VisualStudio기준으로 **멀티바이트 문자집합 or 유니코드 문자집합** 어떤걸 설정했냐에 따라다르게 된다.<br>
🌎 [참고사이트](https://taehyungs-programming-blog.github.io/blog/docs/cpp/win32api/2022-01-13-win32-7/#unicode%EC%97%90-%EA%B4%80%ED%95%98%EC%97%AC)<br>

👀 **멀티바이트 문자집합**을 세팅한 경우 **[MBCS(Multi Byte Character Set)]** : 하나의 문자를 표현하기 위해 다양한 바이트 수를 사용, 윈도우OS에서 영어는 1바이트, 한글은 2바이트 (보통 2바이트 정로를 사용하기에 **DBCS(Double Bytes Character Set)**이라고도 한다.)<br>

```cpp
int main()
{
    char s[] = "abcd가나다라";
    printf("%d\n", sizeof(s));  // 13(abcd(4바이트) + 가나다라(8바이트) + 마지막에 null 포함)
    printf("%d\n", strlen(s));  // 12
    // 이런 연산이 불가능
    char* p = s;
    while(*p != 0)
    {
        printf("%s\n", p);
        p = p + 1;
        /*
		(!!단점!!)
        영어는 1바이트만 추가해도 다음 문자열이 나오는데,
        한글은 2바이트를 추가해야 다음 문자열을 출력할 수 있다.
        */
    }
}
```
👀 조금 정리하자면?<br>
👀 **멀티바이트 문자집합**으로는 하나의 문자를 1, 2bytes로 표현하기에 프로그램 작성시 문제점이 있다.<br>
👀 **나라별로 표준(시스템 로캘을 변경시 달라진다)**이 다르기에 호환성 문제가 있다.<br>
👀 결론 : 전세계 대부분의 문자를 동시에 표현할 수 있는 새로운 문자 집합이 필요하다 👉 **Unicode**<br>

👀 **유니코드**<br>
👀 **UTF-8** : 가변 길이로 인코딩, 1,2,3,4 바이트 모두 사용 언어에 따라 다름(영어 1, 한글 3바이트 사용) 아무래도 영어를 많이 사용하는 환경에서 사용<br>
👀 **UTF-16** : 가변 길이로 인코딩, 대부분 문자 2바이트 특정문자 4바이트(영어 2, 한글 2바이트 사용) 한글과 영어가 비슷하게 사용되는 환경에서 사용<br>
👀 **UTF-32** : 모든 문자를 4바이트로 인코딩, 단 메모리 사용량 증가 리눅스에서 사용<br>
👀 Windows OS는 UTF-16을 사용<br>
👀 Linux의 경우 UTF-32임 참고<br>

```cpp
#include <stdio.h>
#include <string.h>
int main()
{
    char s1[] = "abcd가나다라"; // DBCS
    wchar_t s2[] = L"abcd가나다라";
    // L"" -> 문자열을 유니코드로 만들어 달라 -> Windows의 경우 UTF-16이고, 2byte
    /*
    whar_t :
        Windows : 2byte
        Linux : 4byte, 리눅스의 경우 UTF-32를 쓰니 그렇겠지?
    */
    printf("%d\n", sizeof(s2));         // 18(null포함)
    printf("%d\n", strlen((char*)s2));  // 1 -> why?
    // 제일 처음 나오는 문자 a는 아스키코드로 65이고 2바이트로 표현시 65, 00
    // little endian 표현으로 65, 00로 표현하게 되는데
    // strlen은 00(null)을 만날시 마지막 문자로 인식해서 길이를 리턴하게 된다.
    // 유니코드의 문자열 개수를 세는 새로운 len함수가 필요하다
}
```

---

### 멀티바이트 문자집합 Vs 유니코드 문자집합

* 멀티바이트 문자집합 / MBCS(Multi Byte Character Set) : (char) 개별 문자를 다수의 바이트로 표현(시스템 로캘을 바탕으로 문자집합을 기준으로 문자를 파싱)
* 유니코드 문자집합 / WBCS(Wide Byte Character Set) : (wchar) 유니코드 기반의 character set(UTF-16)

* [참고사이트 🌍](https://taehyungs-programming-blog.github.io/blog/docs/cpp/win32api/2022-01-13-win32-7/#unicode%EC%97%90-%EA%B4%80%ED%95%98%EC%97%AC)

```cpp
char snedData[1000] = "가";         // 시스템 로캘 한글기준 / CP949 (한글2바이트, 로마자1바이트)
char snedData[1000] = u8"가";       // UTF8 (Unicode)
WCHAR snedData[1000] = L"가";       // UTF16 (Unicode)
TCHAR snedData[1000] = _T("가");
```

---

## 구현


```cpp
WCHAR sendData3[1000] = L"가"; // UTF16 = Unicode (한글/로마 2바이트)

while (true)
{
    vector<BuffData> buffs{ BuffData {100, 1.5f}, BuffData{200, 2.3f}, BuffData {300, 0.7f } };
    SendBufferRef sendBuffer = ServerPacketHandler::Make_S_TEST(1001, 100, 10, buffs, L"안녕하세요");
    GSessionManager.Broadcast(sendBuffer);

    this_thread::sleep_for(250ms);
}
```

```cpp
SendBufferRef ServerPacketHandler::Make_S_TEST(uint64 id, uint32 hp, uint16 attack, vector<BuffData> buffs, wstring name)
{
	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);

	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());

	PacketHeader* header = bw.Reserve<PacketHeader>();
	// id(uint64), 체력(uint32), 공격력(uint16)
	bw << id << hp << attack;

	// 가변 데이터
	bw << (uint16)buffs.size();
	for (BuffData& buff : buffs)
	{
		bw << buff.buffId << buff.remainTime;
	}

	bw << (uint16)name.size();
	bw.Write((void*)name.data(), name.size() * sizeof(WCHAR));

	header->size = bw.WriteSize();
	header->id = S_TEST; // 1 : Test Msg

	sendBuffer->Close(bw.WriteSize());

	return sendBuffer;
}
```

```cpp
void ClientPacketHandler::Handle_S_TEST(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PacketHeader header;
	br >> header;

	uint64 id;
	uint32 hp;
	uint16 attack;
	br >> id >> hp >> attack;

	cout << "ID: " << id << " HP : " << hp << " ATT : " << attack << endl;

	vector<BuffData> buffs;
	uint16 buffCount;
	br >> buffCount;

	buffs.resize(buffCount);
	for (int32 i = 0; i < buffCount; i++)
	{
		br >> buffs[i].buffId >> buffs[i].remainTime;
	}

	cout << "BufCount : " << buffCount << endl;
	for (int32 i = 0; i < buffCount; i++)
	{
		cout << "BufInfo : " << buffs[i].buffId << " " << buffs[i].remainTime << endl;
	}

	wstring name;
	uint16 nameLen;
	br >> nameLen;
	name.resize(nameLen);

	br.Read((void*)name.data(), nameLen * sizeof(WCHAR));

	wcout.imbue(std::locale("kor"));
	wcout << name << endl;
}
```