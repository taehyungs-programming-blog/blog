---
layout: default
title: "overlay port"
parent: "(vcpkg)"
grand_parent: "Etc 🛠"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## vcpkg 특정 라이브러리 빌드 실패 시 Overlay Port로 해결하기 (builtin-baseline 환경)

* vcpkg를 사용하다 보면 cmake의 압축 해제 실패 등으로 특정 라이브_러리_ 설치에 계속 실패하는 경우가 있다. 
* 특히, 프로젝트의 안정성을 위해 vcpkg.json의 builtin-baseline으로 vcpkg 버전을 고정해 둔 상태라면 vcpkg를 무작정 업데이트할 수도 없어 문제가 더욱 까다로워진다.

* 이럴 때 **오버레이 포트(Overlay Ports)** 기능을 사용하면, vcpkg의 기본 동작을 우리가 원하는 대로 재정의(override)하여 문제를 해결할 수 있다.

> 이 글에서는 pugixml 라이브러리가 압축 해제 오류로 빌드에 실패하는 상황을 가정하고, 오버레이 포트를 이용해 수동으로 빌드하는 방법을 단계별로 정리한다.

### 문제 상황

* Visual Studio에서 프로젝트를 빌드할 때, vcpkg가 pugixml 라이브러리를 설치하는 과정에서 아래와 같은 CMake Error가 발생하며 빌드에 실패.

```
CMake Error at scripts/cmake/vcpkg_execute_required_process.cmake:127 (message):
    Command failed: "D:/Program Files/CMake/bin/cmake.exe" -E tar xjf ...
EXEC : error : building pugixml:x64-windows failed with: BUILD_FAILED

```

### 해결 방법: Overlay Port 구성

* vcpkg가 자동으로 소스 코드를 다운로드하고 압축 해제하는 과정을 건너뛰고, 우리가 직접 준비한 소스 코드를 사용하도록 설정을 재정의.

* 소스코드 수동 준비
    * 먼저, 문제가 되는 라이브러리의 소스 코드를 vcpkg가 다운로드하는 경로(vcpkg/downloads)에서 직접 압축 해제.
* vcpkg/downloads 폴더로 이동합니다.
    * zeux-pugixml-v1.15.tar.gz 와 같은 파일을 7-Zip 등의 압축 프로그램으로 두 번 압축 해제하여 pugixml-1.15 와 같은 폴더 형태로 만든다.
* vcpkg-configuration.json 파일 생성
    * vcpkg에게 우리가 만든 오버레이 포트의 위치를 알려주기 위해, 프로젝트 최상위 폴더에 vcpkg-configuration.json 파일을 생성하고 아래 내용을 작성.

```
D:\Project\
|-- vcpkg-configuration.json  <-- 여기!
|-- my-overlay-ports/
|-- vcpkg.json
|-- ...
```

* `vcpkg-configuration.json` 내

```json
{
  "overlay-ports": [
    "./my-overlay-ports"
  ]
}
```

* **overlay-ports**: vcpkg가 기본 라이브러리 목록을 읽기 전에 먼저 확인할 사용자 정의 포트 폴더 목록을 지정.

* 오버레이 포트 폴더 및 파일 생성
    * 이제 실제 재정의 내용을 담을 폴더와 파일들을 생성.

* 프로젝트 최상위 폴더에 my-overlay-ports 라는 이름의 폴더를 생성.
    * my-overlay-ports 폴더 안에, 재정의할 라이브러리 이름과 동일한 pugixml 폴더를 생성.

* pugixml 폴더 안에 portfile.cmake와 vcpkg.json 두 개의 파일을 생성.

* 최종 폴더 구조:

```
D:\Project\
|-- my-overlay-ports/
|   |-- pugixml/
|       |-- portfile.cmake    <-- 빌드 스크립트
|       |-- vcpkg.json        <-- 버전 정보
|
|-- vcpkg-configuration.json
|-- ...

```

* portfile.cmake 와 vcpkg.json 작성
    * vcpkg.json
    * pugixml 폴더 안에 있는 vcpkg.json 파일에는 재정의할 라이브러리의 이름과 버전 정보를 간략하게 작성.

```json
{
  "name": "pugixml",
  "version-string": "1.15"
}

```

* portfile.cmake
    * 가장 핵심적인 파일
    * pugixml 폴더 안의 portfile.cmake 파일에 vcpkg가 이 라이브러리를 어떻게 빌드해야 할지 상세한 지침을 작성.

* builtin-baseline으로 고정된 **아주 오래된 버전의 vcpkg**는 vcpkg_from_local이나 vcpkg_cmake_configure 같은 최신 명령어를 인식하지 못함. 
* 따라서 아래와 같이 구형 명령어들로 스크립트를 작성.

```cmake
# vcpkg의 공용 함수 스크립트를 불러옵니다. 구 버전에서는 필수입니다.
include(vcpkg_common_functions)

# 1. 수동으로 압축 해제한 소스 코드의 경로를 변수로 지정합니다.
#    경로 구분자는 `\` 대신 `/`를 사용해야 안정적입니다.
set(SOURCE_PATH "D:/Developer/Projects/HonglabVulkan-main/vcpkg/downloads/pugixml-1.15")

# 2. CMake를 설정합니다. (구 버전 명령어: vcpkg_configure_cmake)
#    SOURCE_PATH를 지정하여 다운로드 및 압축 해제 단계를 건너뜁니다.
vcpkg_configure_cmake(
    SOURCE_PATH ${SOURCE_PATH}
    OPTIONS
        # 필요에 따라 빌드 옵션을 추가합니다.
        -DBUILD_SHARED_LIBS=OFF
)

# 3. 빌드를 수행하고 설치합니다. (구 버전 명령어: vcpkg_install_cmake)
vcpkg_install_cmake()

# 4. 설치된 CMake 타겟 파일들의 경로를 수정해줍니다.
vcpkg_fixup_cmake_targets(CONFIG_PATH "lib/cmake/pugixml")

# 5. 디버그 빌드 시 생성되는 불필요한 파일을 제거합니다.
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

# 6. 라이선스 파일을 수동으로 복사하고 이름을 'copyright'으로 변경합니다.
file(COPY "${SOURCE_PATH}/LICENSE.md" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
file(RENAME "${CURRENT_PACKAGES_DIR}/share/${PORT}/LICENSE.md" "${CURRENT_PACKAGES_DIR}/share/${PORT}/copyright")
```
