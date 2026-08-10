---
layout: default
title: "[구현] 패킷직렬화-2"
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

* [Get This Code 🌎](https://github.com/EasyCoding-7/Windows_Game_Server_Tutorial/tree/RA-Tag-26)

---

## 문제

🍅 이번 강의 내용은 크게 없음, 간단히 **성능개선**만 해볼 예정<br>
🍅 현재 코드에도 약간의 문제가 있는데

```cpp
while (true)
{
    // buffs라는 임시객체를 쓰게 되는데
    vector<BuffData> buffs{ BuffData {100, 1.5f}, BuffData{200, 2.3f}, BuffData {300, 0.7f } };
    SendBufferRef sendBuffer = ServerPacketHandler::Make_S_TEST(1001, 100, 10, buffs, L"안녕하세요");
```

```cpp
// Make_S_TEST내부에서도 buffs를 데이터를 넣는 용도로만 쓰지 재활용이 되지 않는다.

SendBufferRef ServerPacketHandler::Make_S_TEST(uint64 id, uint32 hp, uint16 attack, vector<BuffData> buffs, wstring name)
{
	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);

	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());

	PacketHeader* header = bw.Reserve<PacketHeader>();
	// id(uint64), 체력(uint32), 공격력(uint16)
	bw << id << hp << attack;

	struct ListHeader
	{
		uint16 offset;
		uint16 count;
	};

	// 가변 데이터
	ListHeader* buffsHeader = bw.Reserve<ListHeader>();

	buffsHeader->offset = bw.WriteSize();
	buffsHeader->count = buffs.size();

	for (BuffData& buff : buffs)
		bw << buff.buffId << buff.remainTime;

	header->size = bw.WriteSize();
	header->id = S_TEST; // 1 : Test Msg

	sendBuffer->Close(bw.WriteSize());

	return sendBuffer;
}
```

🍅 개선해 보자

---

## 개선

```cpp
// 그냥 buffer의 주소를 받고
void ClientPacketHandler::Handle_S_TEST(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PKT_S_TEST* pkt = reinterpret_cast<PKT_S_TEST*>(buffer);

	if (pkt->Validate() == false)
		return;

    // buffer를 그냥 바로 사용하도록 하자.
	PKT_S_TEST::BuffsList buffs = pkt->GetBuffsList();

    // ...
```

🍅 Read는 Buffer에서 바로 읽는게 구현이 좀 쉬운데 Write는 조금 까다로운 부분이 있어서 다음강에서 진행