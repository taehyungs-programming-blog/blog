---
layout: default
title: "01-04. FInternetAddrBSD"
parent: "([Network] 01.SocketSubsystemModule)"
grand_parent: "(UE SourceCode 분석 🤖)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* `FSocketWindows`에서는 주소값을 어떻게 관리 할까?

```cpp
/** 
 * represents an internet ip address with support for ipv4/v6
 * all data is in network byte order
 */
/* 
 * IPv4/v6를 지원하는 인터넷 IP 주소를 나타냅니다
 * 모든 데이터는 네트워크 바이트 순서입니다
 */


// in UDP, InternetAddrBSD usage is important:
// - recall how the UDP works: VERY SIMPLE, just set ip-address(and port) and shoot the packet:
//   - whether the receiver successfuly received the packet or not is NOT important for UDP

// UDP에서 InternetAddrBSD 사용은 중요합니다:
// - UDP 작동 방식을 상기해보세요: 매우 간단합니다, IP 주소(와 포트)를 설정하고 패킷을 전송하면 됩니다:
//   - 수신자가 패킷을 성공적으로 받았는지 여부는 UDP에서 중요하지 않습니다
class FInternetAddrBSD : public FInternetAddr
{
    FInternetAddrBSD()
    {
        SocketSubsystem = nullptr;
        Clear();
    }

    virtual void Clear()
    {
        FMemory::Memzero(&Addr, sizeof(Addr));

        // ss_family is indicator what type of address protocol used: e.g. IPv4 or IPv6
        // ss_family는 사용된 주소 프로토콜 유형을 나타내는 지표입니다: 예를 들어 IPv4 또는 IPv6
        Addr.ss_family = AF_UNSPEC;
    }

    // ... 기존 코드 ...

    /** the internet ip address structure */
    // note that sockaddr_storage is available in both windows/linux
    // - if you want to know the detail, see ws2def.h in windows platform

    /** 인터넷 IP 주소 구조체 */
    // sockaddr_storage는 윈도우와 리눅스 모두에서 사용 가능합니다
    // - 자세한 내용을 알고 싶다면 윈도우 플랫폼의 ws2def.h를 참조하세요
    sockaddr_storage Addr;

    /** the subsystem that created this address */
    /** 이 주소를 생성한 서브시스템 */
    FSocketSubsystemBSD* SocketSubsystem;
};
```