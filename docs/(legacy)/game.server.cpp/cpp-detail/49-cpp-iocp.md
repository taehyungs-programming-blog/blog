---
layout: default
title: "[구현] 패킷직렬화-3"
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

* [Get This Code 🌎](https://github.com/EasyCoding-7/Windows_Game_Server_Tutorial/tree/RA-Tag-27)

---


🐶 Send Buffer에 데이터를 바로 써서 Send하는 기능을 구현해 보자.

```cpp
// 대략 이렇게 쓰고 싶음.

WCHAR sendData3[1000] = L"가"; // UTF16 = Unicode (한글/로마 2바이트)

while (true)
{
    // [ PKT_S_TEST ]
    PKT_S_TEST_WRITE pktWriter(1001,    // id
                                100,    // hp
                                10);    // attack

    // [ PKT_S_TEST ][BuffsListItem BuffsListItem BuffsListItem]
    // 버퍼 리스트를 3개 할당.
    PKT_S_TEST_WRITE::BuffsList buffList = pktWriter.ReserveBuffsList(3);

    buffList[0] = { 100, 1.5f };
    buffList[1] = { 200, 2.3f };
    buffList[2] = { 300, 0.7f };

    PKT_S_TEST_WRITE::BuffsVictimsList vic0 = pktWriter.ReserveBuffsVictimsList(&buffList[0], 3);
    {
        // 내부적으로 다시 할당 가능.
        vic0[0] = 1000;
        vic0[1] = 2000;
        vic0[2] = 3000;
    }

    PKT_S_TEST_WRITE::BuffsVictimsList vic1 = pktWriter.ReserveBuffsVictimsList(&buffList[1], 1);
    {
        vic1[0] = 1000;
    }

    PKT_S_TEST_WRITE::BuffsVictimsList vic2 = pktWriter.ReserveBuffsVictimsList(&buffList[2], 2);
    {
        vic2[0] = 3000;
        vic2[1] = 5000;
    }

    SendBufferRef sendBuffer = pktWriter.CloseAndReturn();
```

```cpp
BuffsList ReserveBuffsList(uint16 buffCount)
{
    BuffsListItem* firstBuffsListItem = _bw.Reserve<BuffsListItem>(buffCount);
    _pkt->buffsOffset = (uint64)firstBuffsListItem - (uint64)_pkt;
    _pkt->buffsCount = buffCount;
    return BuffsList(firstBuffsListItem, buffCount);
}
```

---

## 분석

```cpp
class PKT_S_TEST_WRITE
{
public:
	using BuffsListItem = PKT_S_TEST::BuffsListItem;
	using BuffsList = PacketList<PKT_S_TEST::BuffsListItem>;
	using BuffsVictimsList = PacketList<uint64>;

    // PKT_S_TEST_WRITE pktWriter(1001, 100, 10);
        // 생성이 되면 고정데이터인 id, hp, attack을 받아서 처리(여기까진 쉽다)
	PKT_S_TEST_WRITE(uint64 id, uint32 hp, uint16 attack)
	{
		_sendBuffer = GSendBufferManager->Open(4096);
		_bw = BufferWriter(_sendBuffer->Buffer(), _sendBuffer->AllocSize());

		_pkt = _bw.Reserve<PKT_S_TEST>();
		_pkt->packetSize = 0; // To Fill
		_pkt->packetId = S_TEST;
		_pkt->id = id;
		_pkt->hp = hp;
		_pkt->attack = attack;
		_pkt->buffsOffset = 0; // To Fill
		_pkt->buffsCount = 0; // To Fill
	}
```

```cpp
class PKT_S_TEST_WRITE
{
    // 사용될 자료형이 세 개있는데
public:
	using BuffsListItem = PKT_S_TEST::BuffsListItem;
    /*
        struct PKT_S_TEST
        {
            struct BuffsListItem
            {
                uint64 buffId;
                float remainTime;

                // Victim List
                uint16 victimsOffset;
                uint16 victimsCount;
            };
    */
	using BuffsList = PacketList<PKT_S_TEST::BuffsListItem>;
	using BuffsVictimsList = PacketList<uint64>;

    // BuffsList, BuffsVictimsList는 PacketList라는 template으로 관리된다.
    /*
        template<typename T>
        class PacketList
        {
        public:
            PacketList() : _data(nullptr), _count(0) { }
            PacketList(T* data, uint16 count) : _data(data), _count(count) { }

            T& operator[](uint16 index)
            {
                ASSERT_CRASH(index < _count);
                return _data[index];
            }

            uint16 Count() { return _count; }

            // ranged-base for 지원
            PacketIterator<T, PacketList<T>> begin() { return PacketIterator<T, PacketList<T>>(*this, 0); }
            PacketIterator<T, PacketList<T>> end() { return PacketIterator<T, PacketList<T>>(*this, _count); }

        private:
            T*          _data;
            uint16		_count;
        };
    */
```

🐶 데이터를 넣는 부분을 보자면

```cpp
// using BuffsList = PacketList<PKT_S_TEST::BuffsListItem>;
PKT_S_TEST_WRITE::BuffsList buffList = pktWriter.ReserveBuffsList(3);
/*
    BuffsList ReserveBuffsList(uint16 buffCount)
    {
        // write buffer에 buffcount * BuffsListItem 만큼 할당
        BuffsListItem* firstBuffsListItem = _bw.Reserve<BuffsListItem>(buffCount);

        _pkt->buffsOffset = (uint64)firstBuffsListItem - (uint64)_pkt;
        _pkt->buffsCount = buffCount;
        return BuffsList(firstBuffsListItem, buffCount);

        /* ******************
            template<typename T>
            class PacketList
            {
            public:
                PacketList() : _data(nullptr), _count(0) { }
                PacketList(T* data, uint16 count) : _data(data), _count(count) { }

                T& operator[](uint16 index)
                {
                    ASSERT_CRASH(index < _count);
                    return _data[index];
                }

                uint16 Count() { return _count; }

                // ranged-base for 지원
                PacketIterator<T, PacketList<T>> begin() { return PacketIterator<T, PacketList<T>>(*this, 0); }
                PacketIterator<T, PacketList<T>> end() { return PacketIterator<T, PacketList<T>>(*this, _count); }

            private:
                T*          _data;
                uint16		_count;
            };
        /* ******************
    }
*/

// 이런 접근이 가능한게 신기할텐데
buffList[0] = { 100, 1.5f };
buffList[1] = { 200, 2.3f };
buffList[2] = { 300, 0.7f };

// buffList[0], [1], [2] 하나하나는 PacketList이고
// PacketList내부에는 operator[]가 재정의 됨
// 재정의된 operator[]는 T* _data;를 리턴해준다.

// 그럼 return _data[index];를 하면 정확히 다음 데이터를 넘겨줄수 있다는말? 직렬로 데이터가 있는가?
// using BuffsList = PacketList<PKT_S_TEST::BuffsListItem>; 이고
// T = PKT_S_TEST::BuffsListItem가 된다.
// Reserve당시에 직렬로 데이터를 할당해 둠.
// BuffsListItem* firstBuffsListItem = _bw.Reserve<BuffsListItem>(buffCount);
```

---
---

## 복습, 수신 부분은?

```cpp
void ClientPacketHandler::Handle_S_TEST(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PKT_S_TEST* pkt = reinterpret_cast<PKT_S_TEST*>(buffer);

	if (pkt->Validate() == false)
		return;

	//cout << "ID: " << id << " HP : " << hp << " ATT : " << attack << endl;

	PKT_S_TEST::BuffsList buffs = pkt->GetBuffsList();
	
	cout << "BufCount : " << buffs.Count() << endl;

	for (auto& buff : buffs)
	{
		cout << "BufInfo : " << buff.buffId << " " << buff.remainTime << endl;

		PKT_S_TEST::BuffsVictimsList victims =  pkt->GetBuffsVictimList(&buff);

		cout << "Victim Count : " << victims.Count() << endl;

		for (auto& victim : victims)
		{
			cout << "Victim : " << victim << endl;
		}

	}
}
```

---

## 분석2

```cpp
while (true)
{
    // [ PKT_S_TEST ]
    PKT_S_TEST_WRITE pktWriter(1001,    // id
                                100,    // hp
                                10);    // attack

    // [ PKT_S_TEST ][BuffsListItem BuffsListItem BuffsListItem]
    PKT_S_TEST_WRITE::BuffsList buffList = pktWriter.ReserveBuffsList(3);
    buffList[0] = { 100, 1.5f };
    buffList[1] = { 200, 2.3f };
    buffList[2] = { 300, 0.7f };

    // buffList라는 리스트 안에 또 vic0라는 리스트가 들어간 형태이다.
        // 어떻게 처리되는지 궁금
    PKT_S_TEST_WRITE::BuffsVictimsList vic0 = pktWriter.ReserveBuffsVictimsList(&buffList[0], 3);
    {
        vic0[0] = 1000;
        vic0[1] = 2000;
        vic0[2] = 3000;
    }

    PKT_S_TEST_WRITE::BuffsVictimsList vic1 = pktWriter.ReserveBuffsVictimsList(&buffList[1], 1);
    {
        vic1[0] = 1000;
    }

    PKT_S_TEST_WRITE::BuffsVictimsList vic2 = pktWriter.ReserveBuffsVictimsList(&buffList[2], 2);
    {
        vic2[0] = 3000;
        vic2[1] = 5000;
    }

    SendBufferRef sendBuffer = pktWriter.CloseAndReturn();
```

```cpp
PKT_S_TEST_WRITE::BuffsList buffList = pktWriter.ReserveBuffsList(3);
/*
	BuffsList ReserveBuffsList(uint16 buffCount)
	{
		BuffsListItem* firstBuffsListItem = _bw.Reserve<BuffsListItem>(buffCount);

        // 리스트의 시작주소(firstBuffsListItem)에서 해당 패킷의 주소를(_pkt) 뺀다
            // 옵셋값이 나오고 저장.
		_pkt->buffsOffset = (uint64)firstBuffsListItem - (uint64)_pkt;
		_pkt->buffsCount = buffCount;
		return BuffsList(firstBuffsListItem, buffCount);
	}
*/
buffList[0] = { 100, 1.5f };
buffList[1] = { 200, 2.3f };
buffList[2] = { 300, 0.7f };
```

```cpp
PKT_S_TEST_WRITE::BuffsVictimsList vic0 = pktWriter.ReserveBuffsVictimsList(&buffList[0], 3);
/*
	BuffsVictimsList ReserveBuffsVictimsList(BuffsListItem* buffsItem, uint16 victimsCount)
	{
		uint64* firstVictimsListItem = _bw.Reserve<uint64>(victimsCount);

        // 위와 동일
		buffsItem->victimsOffset = (uint64)firstVictimsListItem - (uint64)_pkt;
		buffsItem->victimsCount = victimsCount;
		return BuffsVictimsList(firstVictimsListItem, victimsCount);
	}
*/
{
    vic0[0] = 1000;
    vic0[1] = 2000;
    vic0[2] = 3000;
}
```

```cpp
// 사용은
BuffsList GetBuffsList()
{
    BYTE* data = reinterpret_cast<BYTE*>(this);

    // 패킷의 시작주소에서 옵셋값만큼을 더하여 buffList주소라고 알려준다.
    data += buffsOffset;
    return BuffsList(reinterpret_cast<PKT_S_TEST::BuffsListItem*>(data), buffsCount);
}

// vicitem도 마찬가지
BuffsVictimsList GetBuffsVictimList(BuffsListItem* buffsItem)
{
    BYTE* data = reinterpret_cast<BYTE*>(this);
    data += buffsItem->victimsOffset;
    return BuffsVictimsList(reinterpret_cast<uint64*>(data), buffsItem->victimsCount);
}
```