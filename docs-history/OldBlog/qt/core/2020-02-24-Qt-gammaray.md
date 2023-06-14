---
title: "(Qt) Gammaray"
date: 2020-02-24 00:00:00 -0000
---

> 아래 스크립트를 수정해서 사용하면 된다.

```s
cd C:\QtCreatorProjects\GammaRayBuild
set git_path=C:\Program Files\Git\bin
set cmake_path=C:\Program Files\CMake\bin
set qt_path=C:\Qt\5.12.0\msvc2015_64
set compiler_path=C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC
set comp_platform=amd64
set originalWD=%CD%
set PATH=%cmake_path%;%git_path%;%PATH%
call "%qt_path%\bin\qtenv2.bat"
cd %originalWD%
echo on
call "%compiler_path%\vcvarsall.bat" %comp_platform%
set CMAKEGENERATOR="NMake Makefiles"
git clone --depth=1 git://anongit.kde.org/extra-cmake-modules ECMbuild
cd ECMbuild
mkdir build 
cd build
cmake -G %CMAKEGENERATOR% -DBUILD_TESTING=OFF -DCMAKE_INSTALL_PREFIX=../../ECM -DCMAKE_BUILD_TYPE=RELEASE ../
cmake --build .
cmake --build . --target install
cd ..\..\
set PATH=%CD%\ECM;%PATH%
git clone --depth=1 git://anongit.kde.org/kcoreaddons.git KCoreAddonbuild
cd KCoreAddonbuild
mkdir build
cd build
cmake -G %CMAKEGENERATOR% -DBUILD_TESTING=OFF -DCMAKE_INSTALL_PREFIX=../../KCoreAddon -DCMAKE_BUILD_TYPE=RELEASE ../
cmake --build .
cmake --build . --target install
cd ..\..\
set PATH=%CD%\KCoreAddon;%PATH%
git clone --depth=1 git://anongit.kde.org/syntax-highlighting.git KSyntaxHighlightingbuild
cd KSyntaxHighlightingbuild
mkdir build
cd build
cmake -G %CMAKEGENERATOR% -DBUILD_TESTING=OFF -DCMAKE_INSTALL_PREFIX=../../KSyntaxHighlighting -DCMAKE_BUILD_TYPE=RELEASE ../
cmake --build .
cmake --build . --target install
cd ..\..\
set PATH=%CD%\KSyntaxHighlighting;%PATH%
git clone --depth=1 https://github.com/KDAB/GammaRay.git GammaRaybuild
cd GammaRaybuild
mkdir build
cd build
cmake -G %CMAKEGENERATOR% -DBUILD_TESTING=OFF -DCMAKE_INSTALL_PREFIX=../../GammaRay -DCMAKE_BUILD_TYPE=RELEASE ../
cmake --build .
cmake --build . --target install
cd ..\..\
```

> 수정할 부분은 아래 6줄

```s
cd C:\QtCreatorProjects\GammaRayBuild
set git_path=C:\Program Files\Git\bin
set cmake_path=C:\Program Files\CMake\bin
set qt_path=C:\Qt\5.12.0\msvc2015_64
set compiler_path=C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC
set comp_platform=amd64
```

> 대부분은 보면 알것 같고<br>
> `set comp_platform=amd64`은 x86, amd64 중 하나를 선택하면 된다.

---

* [참고사이트](https://forum.qt.io/topic/101240/install-gammaray-on-windows/3)