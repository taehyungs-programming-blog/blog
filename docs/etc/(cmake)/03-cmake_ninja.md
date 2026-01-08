---
layout: default
title: "03. build system (ninja)"
parent: "(CMake 🦊)"
grand_parent: "Etc 🛠"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## MSBuild vs. Ninja: 두 빌드 시스템 비교 분석

### MSBuild (Microsoft Build Engine)란?

* MSBuild는 Visual Studio 프로젝트의 빌드를 위해 설계된 고수준(High-level) 빌드 시스템
* Visual Studio의 솔루션 파일(.sln)과 프로젝트 파일(.vcxproj)을 직접 해석하고, 소스 코드 컴파일, 링킹, 파일 복사 등 빌드에 필요한 모든 작업을 관리하고 실행
    * 입력 파일: .vcxproj (XML 형식의 프로젝트 파일)
    * 핵심 역할: 프로젝트의 구조, 설정, 파일 간의 종속성을 모두 이해하고 빌드 과정을 총괄 지휘

### Ninja란?

* Ninja는 속도를 최우선으로 설계된 저수준(Low-level) 빌드 시스템
* Ninja 자체는 프로젝트의 구조를 해석하지 않는다. 대신, CMake와 같은 **메타 빌드 시스템(Meta-build system)**이 생성해준 간단한 빌드 스크립트(build.ninja)를 읽어, 명령어를 가장 빠르게 실행하는 역할에만 집중
    * 입력 파일: build.ninja (간단한 규칙 기반의 텍스트 파일)
    * 핵심 역할: 이미 작성된 빌드 계획서를 가장 빠른 속도로 실행. 종속성 검사 속도가 매우 빠름.

---

| 구분 | MSBuild | Ninja |
|:---|:---|:---|
| 철학 | "All-in-one" 빌드 플랫폼. 프로젝트 관리와 실행을 모두 담당. | "Do one thing well". 빌드 스크립트의 빠른 실행에만 집중. |
| 입력 파일 형식 | 복잡한 XML (.vcxproj) | 매우 단순한 텍스트 (build.ninja) |
| 입력 파일 생성 주체 | Visual Studio IDE 또는 개발자가 직접 관리 |CMake, Meson 등 메타 빌드 시스템이 자동 생성 |
| 종속성 검사 속도 | 대규모 프로젝트에서 상대적으로 느릴 수 있음 | 매우 빠름. 이것이 Ninja의 핵심 장점. |
| 주요 강점 | Visual Studio IDE와의 완벽한 통합, 풍부한 빌드 기능 | 압도적으로 빠른 증분 빌드(Incremental Build) 속도, 플랫폼 독립성 |
| 주요 사용 환경 | Windows 및 Visual Studio 중심의 개발 | 크로스 플랫폼(Windows, Linux, macOS) 프로젝트, 대규모 코드베이스 |

---

### MSbuidl Example

```xml
<!-- MyProject.vcxproj (일부 발췌) -->
<Project ...>
  <ItemGroup>
    <ClCompile Include="main.cpp" />
    <ClCompile Include="utils.cpp" />
  </ItemGroup>
  <ItemDefinitionGroup>
    <ClCompile>
      <!-- 컴파일러 옵션 -->
      <WarningLevel>Level3</WarningLevel>
    </ClCompile>
  </ItemDefinitionGroup>
</Project>

```

* msbuild.exe가 .vcxproj 파일을 파싱하여 빌드해야 할 파일과 적용할 옵션을 결정
* 결정된 내용에 따라 cl.exe(컴파일러)와 link.exe(링커)를 순서대로 호출

---

### ninja

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(MyProject)

add_executable(MyProject main.cpp utils.cpp)
```

```ninja
# build.ninja (일부 발췌)
rule CXX_COMPILER
  command = C:\path\to\cl.exe /nologo /TP $DEFINES $INCLUDES $FLAGS /showIncludes /Fo$out /Fd$TARGET_COMPILE_PDB /FS -c $in
  description = Building CXX object $out

build CMakeFiles/MyProject.dir/main.cpp.obj: CXX_COMPILER ..\main.cpp
build CMakeFiles/MyProject.dir/utils.cpp.obj: CXX_COMPILER ..\utils.cpp

rule LINKER
  command = cmd.exe /C "cd . && C:\path\to\cmake.exe -E vs_ lancement MyProject.exe"
  ...

build MyProject.exe: LINKER CMakeFiles/MyProject.dir/main.cpp.obj CMakeFiles/MyProject.dir/utils.cpp.obj
```

---

## 정리

* MSBuild
    * MSBuild는 Visual Studio 생태계에 깊숙이 통합된 강력한 빌드 시스템. 
    * Windows 환경에서 Visual Studio를 주로 사용한다면 별도의 설정 없이 편리하게 사용.
* Ninja
    * Ninja는 순수한 '실행' 속도에만 집중하는 도구. 
    * 이 때문에 CMake와 같은 메타 빌드 시스템과 함께 사용. 
    * 빌드 속도가 매우 중요하거나, 여러 플랫폼을 동시에 지원해야 하는 대규모 프로젝트에 절대적으로 유리.