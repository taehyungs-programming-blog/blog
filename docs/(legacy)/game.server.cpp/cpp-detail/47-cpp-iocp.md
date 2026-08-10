---
layout: default
title: "[구현] 패킷직렬화-1"
parent: "(C++) 상세 구현"
grand_parent: "Game Server 👾"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get This Code 🌎](https://github.com/EasyCoding-7/Windows_Game_Server_Tutorial/tree/RA-Tag-25)

---

## 들어가며, 패킷직렬화란?

🦩 들어가기 전 질문

```cpp
struct PKT_S_TEST
{
    uint32 hp;      // 4
    uint64 id;      // 8
    uint16 attack;  // 2
}
```

🦩 총 14바이트가 사용될까?? 👉 24바이트가 사용된다?!<br>
🦩 왜 그럴까?

🦩 64bit운영체제 아래에서는 8바이트 단위로 접근이 속도가 빠르다<br>
🦩 따라서 8바이트 단위로 더미데이터를 넣어준다

```
 |  hp(4)  |  dummy(4)  |
 |      id(8)           |
 | attack(2) | dummy(6) |
```

🦩 컴파일러에게 1바이트 단위로 넣어달라 알려줘야한다.

```cpp
#pragma pack(1)
struct PKT_S_TEST
{
    uint32 hp;      // 4
    uint64 id;      // 8
    uint16 attack;  // 2
}
#pragma pack
```

🦩 항상 패킷을 만들때 `#pragma pack(1)`를 추가하도록 하자.

---

## 현재코드의 문제점

🦩 다시 돌아가서 현재 코드의 문제점을 보자면...

* (문제 1) 서버, 클라가 **패킷의 정보를 별도로 관리**해야만 한다.

```cpp
// 서버/클라 서로가 이 정보를 항상 맞춰야한다.
    // 여기서 그냥 잘 맞추면 되지 않나? 할텐데
    // 만약 서버/클라 언어가 다르다면? + 사소한 실수가 나기 좋은 환경
struct S_TEST
{
	uint64 id;
	uint32 hp;
	uint16 attack;
	// 가변 데이터
        // 1) 문자열 (ex. name)
        // 2) 그냥 바이트 배열 (ex. 길드 이미지)
        // 3) 일반 리스트
	vector<BuffData> buffs;

	wstring name;
};
```

* (문제 2) 패킷이 추가될때마다 **함수가 추가**되어야 하고 실수의 여지가 있다

```cpp
void ClientPacketHandler::Handle_S_TEST(BYTE* buffer, int32 len)
{
    // 데이터가 변경시 매번 이 내부가 같이 변경되어야 함.

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

---

## 해결법

* 당장 패킷직렬화 1~3에서 해결한 문제는 아니고 차차 해결할 예정

🦩 패킷의 정보를 공통으로 관리해보자<br>
🦩 그 방법중 하나는 xml, json 등으로 패킷의 정보를 관리하고 xml, json기반에서 **코드를 뽑아내도록** 만들면 된다.(이걸 앞으로 할 예정)

```xml
<?xml version="1.0" encoding="utf-8"?>
<PDL>
	<Packet name="S_TEST" desc="테스트 용도">
		<Field name="id" type="uint64" desc=""/>
		<Field name="hp" type="uint32" desc=""/>
		<Field name="attack" type="uint16" desc=""/>
		<List name="buffs" desc="">
			<Field name="buffId" type="uint64" desc=""/>
			<Field name="remainTime" type="float" desc=""/>
		</List>
	</Packet>
</PDL>
```

🦩 추가적으로 가변데이터의 처리는 고정데이터를 먼저 놓고 그 뒤에, `uint16 buffsOffset;`, `uint16 buffsCount;` 두 변수를 통해 **가변데이터를 처리**할 예정

```cpp
#pragma pack(1)

// [ PKT_S_TEST ][BuffsListItem BuffsListItem BuffsListItem]
struct PKT_S_TEST
{
	struct BuffsListItem
	{
		uint64 buffId;
		float remainTime;
	};

	uint16 packetSize; // 공용 헤더
	uint16 packetId; // 공용 헤더
	uint64 id; // 8
	uint32 hp; // 4
	uint16 attack; // 2
	uint16 buffsOffset;
	uint16 buffsCount;

	bool Validate()
	{
		uint32 size = 0;
		size += sizeof(PKT_S_TEST);
		size += buffsCount * sizeof(BuffsListItem);
		if (size != packetSize)
			return false;

		if (buffsOffset + buffsCount * sizeof(BuffsListItem) > packetSize)
			return false;

		return true;
	}
	//vector<BuffData> buffs;
	//wstring name;
};
#pragma pack()
```