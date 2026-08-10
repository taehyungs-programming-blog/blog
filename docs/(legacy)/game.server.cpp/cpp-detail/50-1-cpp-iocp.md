---
layout: default
title: "[구현] Protobuf"
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

{% raw %}

🐶 정확한 이해를 위해서 왜 protobuf가 필요한지 이해가 필요한데... 설명이 너무 길어지니 참고사이트를 참고하고, 대략 설명하자면 사용성의 편의를 위해서 protobuf를 사용한다. 고 이해하면 90점은 받는다.

---

## 다운로드

* [protobuf git 🌍](https://github.com/protocolbuffers/protobuf/releases)에서 우선 win64용 protobuf 컴파일러를 다운 받자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/protobuf/protobuf-1-1.png"/>
</p>

🐶 위에서 말하는 컴파일러란 특정 양식으로 패킷을 정의하면 자동으로 코드를 뽑아주는 도구를 컴파일러라 말한다.<br>
🐶 파일의 구조는 아래와 같고 protobuf를 사용할 프로젝트와 함께 두자

```
protobuf
   |
   ------- bin
   |        |
   |        ------- protoc.exe
   |
   ------- include
            |
            ------- google
                       |
                       --------- protobuf
                                    |
                                    ...
```

---

## 간단 테스트

🐶 `protoc.exe`가 있는 폴더내에서 protocol.proto 파일을 생성후 내용을 아래와 같이 만들어 보자.

* 참고) [proto3 문법 🌍](https://developers.google.com/protocol-buffers/docs/proto3)

```
syntax = "proto3";              // proto 버전을 알린다.
package Protocol;               // 패키지 인스톨

// 구조체
message BuffData
{
	uint64 buffId = 1;              // 1은 패킷의 첫 번째에 있다고 알린다.
	float remainTime = 2;
	repeated uint64 victims = 3;    // repeated는 가변데이터를 의미
}

message S_TEST
{
	uint64 id = 1;
	uint32 hp = 2;
	uint32 attack = 3;
	repeated BuffData buffs = 4;
}
```

🐶 위 패킷을 코드로 뽑아보자

```bash
# GenPackets.bat
    # "-I=./" --> .proto 파일이 어디있는지 알림
    # "--cpp_out=./" --> cpp로 뽑아주세요

protoc.exe -I=./ --cpp_out=./ ./Protocol.proto
IF ERRORLEVEL 1 PAUSE   # 에러가 있다면 pause
```

🐶 위 bat파일을 실행해보면 `.cc`, `.h`가 생성된다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/protobuf/protobuf-1-2.png"/>
</p>

🐶 `Protocol.pb.h`, `Protocol.pb.cc`를 열어보면 코드가 엄청긴데 아직은 분석하려 하지말고 우선은 자신이 사용하는 프로젝트에 두 파일을 추가만 하고 빌드가 되게까지만 해보자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/protobuf/protobuf-1-3.png"/>
</p>

* 무조건 빌드가 안될텐데 
    * protobuf에 필요한 라이브러리가 필요하다.
    * 함께 추가했던 protobuf include폴더의 include가 필요하다.

### protobuf에 필요한 라이브러리가 필요하다.

* [probuf github 🌍](https://github.com/protocolbuffers/protobuf)에서 원하는 버전의 branch로 가서 clone을 받자
    * clone시 sub-module 까지 clone해야함을 잊지말자.
* CMAKE를 통해 빌드해보자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/protobuf/protobuf-1-4.png"/>
</p>

🐶 에러가 한 번뜨는데 정상임 아래와 같이 설정하고 다시 Generate<br>
🐶 (추가) 아래와 같이 설정하면 string과 같은 가변문자 처리불가 모두 다 off도 두고 generate할 것.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/protobuf/protobuf-1-5.png"/>
</p>

🐶 아래와 같이 생성된 프로젝트 파일(`protobuf.sln`)을 빌드한다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/protobuf/protobuf-1-6.png"/>
</p>

🐶 생성된 `dll`, `lib`, `pdb` 를 옮긴다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/protobuf/protobuf-1-7.png"/>
</p>

🐶 protobuf를 사용할 프로젝트 내부에 lib의 위치를 알려준다.

```cpp
// pch.h

#ifdef _DEBUG
#pragma comment(lib, "Protobuf\\Debug\\libprotobufd.lib")
#else
#pragma comment(lib, "Protobuf\\Release\\libprotobuf.lib")
#endif
```

---

## 실제로 써보자.

```cpp
while (true)
{
    Protocol::S_TEST pkt;
    pkt.set_id(1000);
    pkt.set_hp(100);
    pkt.set_attack(10);
    {
        // 데이터 내부를 채우는 방법이 아주 다양하게 있음(iterator 쓰는 법 등)
            // 나중에 자세히 알아볼 것.
        Protocol::BuffData* data = pkt.add_buffs();
        data->set_buffid(100);
        data->set_remaintime(1.2f);
        data->add_victims(4000);
    }
    {
        Protocol::BuffData* data = pkt.add_buffs();
        data->set_buffid(200);
        data->set_remaintime(2.5f);
        data->add_victims(1000);
        data->add_victims(2000);
    }

    SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
    GSessionManager.Broadcast(sendBuffer);

    this_thread::sleep_for(250ms);
}
```

```cpp
template<typename T>
SendBufferRef _MakeSendBuffer(T& pkt, uint16 pktId)
{
	const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
	const uint16 packetSize = dataSize + sizeof(PacketHeader);

	SendBufferRef sendBuffer = GSendBufferManager->Open(packetSize);
	PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
	header->size = packetSize;
	header->id = pktId;

    // &header[1]에 dataSize만큼 직렬화로 넣어줘라
	ASSERT_CRASH(pkt.SerializeToArray(&header[1], dataSize));
	sendBuffer->Close(packetSize);

	return sendBuffer;
}
```

```cpp
void ClientPacketHandler::Handle_S_TEST(BYTE* buffer, int32 len)
{
	Protocol::S_TEST pkt;

    // ParseFromArray : byte배열에서 데이터 파싱
	ASSERT_CRASH(pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)));

	cout << pkt.id() << " " << pkt.hp() << " " << pkt.attack() << endl;

	cout << "BUFSIZE : " << pkt.buffs_size() << endl;

	for (auto& buf : pkt.buffs())
	{
		cout << "BUFINFO : " << buf.buffid() << " " << buf.remaintime() << endl;
		cout << "VICTIMS : " << buf.victims_size() << endl;
		for (auto& vic : buf.victims())
		{
			cout << vic << " ";
		}

		cout << endl;
	}
}
```

{% endraw %}
