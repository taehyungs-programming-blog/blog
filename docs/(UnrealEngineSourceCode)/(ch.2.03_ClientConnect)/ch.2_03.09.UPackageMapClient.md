---
layout: default
title: "03-09. FNetConnectionIdHandler"
parent: "([Network] 03. ClientConnect)"
grand_parent: "(UE SourceCode 분석 🤖)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```cpp
// see UPackageMap's comment briefly:
// - we'll cover this in detail near future, but it is nice to know PackageMapClient manages objects and names while communicating over networking

// UPackageMap의 주석을 간단히 살펴보면:
// - 앞으로 자세히 다룰 예정이지만, PackageMapClient가 네트워킹을 통한 통신 중에 객체와 이름을 관리한다는 것을 알아두면 좋습니다

class UPackageMapClient : public UPackageMap
{
public:
    void Initialize(UNetConnection* InConnection, TSharedPtr<FNetGUIDCache> InNetGUIDCache)
    {
        Connection = InConnection;
        GuidCache = InNetGUIDCache;
        ExportNetGUIDCount = 0;
    }

    UNetConnection* Connection;

    TSharedPtr<FNetGUIDCache> GuidCache;

    int32 ExportGUIDCount;
};
```