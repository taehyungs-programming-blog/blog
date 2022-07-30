---
layout: default
title: "QT GUI의 끝판왕 Telegram 분석기"
parent: (QT)
grand_parent: C++
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 빌드(22.01.19 기준)

😺 [Telegram Github 🌍](https://github.com/telegramdesktop/tdesktop) 에서 시키는데로 하면 된다.<br>

🙀 하지만 막히는 부분이 몇 군데 있을 텐데 그 부분만 정리한다.

* jom이 다운이 안됨 👉 한국에서 링크가 막힌거 같음. [여기](https://download.qt.io/official_releases/jom/jom.zip.mirrorlist)서 torrent로 받도록 하자.
* `tdesktop\Telegram\build\prepare\win.bat` 과정에서 `os.environ['Platform']` 👉 [ x64 Native Tools Command Propt for VS 2022 ]로 실행한게 맞는지 다시 확인하자
* breakpad lib 설치중 `UnicodeDecodeError: 'cp949' codec can't decode byte 0xbf in position 2: illegal multibyte sequence`에러가 발생시 시스템언어를 영어로 바꾸고 다시 빌드해보자.

😺 여기까지하면 별 문제없이 빌드가 됨(어어어어어어어어ㅓ어엄청 오래걸림.. 주의)

* 추가로 시스템 로캘이 한글일 경우 빌드가 안됨. 영어로 해두고 디버깅해보시오.

---

## GUI적으로 너무 신기했던 구현 부분 정리

### DPI를 어떻게 유지하는지?

🐱 결론부터 말하자면 ... 아래 방식이 최선이 아니다.<br>
😺 만약 프로그램을 재실행하지 않아야 한다면 아래 방법밖에 없지만. 재실행해도 무방하다면 차라리 아래처럼 사용하는게 낫다.<br>
😺 특히 Qt 6.3.X ~ 이후 버전은 Monitor Scale 125%(1.25), 150%(1.5) 와 같은 소수점 처리도 가능하기에 아래 방법이 더 좋은 방법이라 할 수 있다.

```cpp
// DPI(Scale)의 변화를 처리하도록 설정
SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);

// Scale Factor를 직접 입력하도록 한다.
QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);	// High DPI 쓴다고 선언
qputenv("QT_SCALE_FACTOR", "2");							// 200 % 확대

// (주의) Qt 5.X 버전에서는 정수 High DPI만 설정이 가능함

// 이렇게 쓰는것을 추천... 아래는 코드 수정이 너무너무너무 많음.
```

😺 텔레그램은 `HighDpiScaling`으로 DPI를 컨트롤하진 않는다

```cpp
void Launcher::init() {
	_arguments = readArguments(_argc, _argv);

	prepareSettings();
	initQtMessageLogging();

	QApplication::setApplicationName(qsl("TelegramDesktop"));
	QApplication::setAttribute(Qt::AA_DisableHighDpiScaling, true);
```

😺 그래서 전역으로 `cSetScreenScale(250)` 스케일 값을 저장해 두고 스캐일값에 따라 확대, 축소 시킨다

```cpp
void Sandbox::setupScreenScale() {
	const auto processDpi = [](const QDpi &dpi) {
		return (dpi.first + dpi.second) * qreal(0.5);
	};
	const auto dpi = processDpi(
		Sandbox::primaryScreen()->handle()->logicalDpi());
	LOG(("Primary screen DPI: %1").arg(dpi));
	if (dpi <= 108) {
		cSetScreenScale(100); // 100%:  96 DPI (0-108)
	} else if (dpi <= 132) {
		cSetScreenScale(125); // 125%: 120 DPI (108-132)
	} else if (dpi <= 168) {
		cSetScreenScale(150); // 150%: 144 DPI (132-168)
	} else if (dpi <= 216) {
		cSetScreenScale(200); // 200%: 192 DPI (168-216)
	} else if (dpi <= 264) {
		cSetScreenScale(250); // 250%: 240 DPI (216-264)
	} else {
		cSetScreenScale(300); // 300%: 288 DPI (264-inf)
	}
```

😺 ScreenScale이 어떻게 모든 UI에 반영될까?

```cpp
// 다 설명하면 너무 길고 여차저차 해서 style::Scale()내에 ScreenScale이 저장된다.
// cScale()을 호출하면 현재 Scale정보를 받을 수 있고
// cScale()를 어디서 쓰는지만 보면된다.

inline int cScale() {
	return style::Scale();
}
```

```cpp
// 모든 UI 컴포넌트는 ModuleBase를 상속받고
// virtual void start(int scale) = 0;를 통해 scale을 설정한다

class ModuleBase {
public:
	virtual void start(int scale) = 0;

	virtual ~ModuleBase() = default;

};
```

```cpp
// 예를 하나 들어보자면

class Module_style_chat : public style::internal::ModuleBase {
public:
	Module_style_chat() { style::internal::registerModule(this); }

	void start(int scale) override {
		style::internal::init_style_chat(scale);
	}
};
```

```cpp
void init_style_chat(int scale) {
	if (inited) return;
	inited = true;

	init_style_basic(scale);
	init_style_dialogs(scale);
	init_style_widgets(scale);
	init_style_menu_icons(scale);

	initPxValues(scale);

    // ...
```

```cpp
void initPxValues(int scale) {
	pxm2 = ConvertScale(-2, scale);
	pxm1 = ConvertScale(-1, scale);
	px0 = ConvertScale(0, scale);
    // ...
    px160 = ConvertScale(160, scale);
	px240 = ConvertScale(240, scale);
	px320 = ConvertScale(320, scale);
	px360 = ConvertScale(360, scale);
	px430 = ConvertScale(430, scale);
    // 내가 몇 픽셀까지 쓰는지에 따라 달라진다
```

```cpp
template <typename T>
[[nodiscard]] inline T ConvertScale(T value, int scale) {
	return (value < 0.)
		? (-ConvertScale(-value, scale))
		: T(base::SafeRound((double(value) * scale / 100.) - 0.01));
}
```

#### 참고) 텔레그램에서 전역변수를 관리하는 방법

```cpp
// settings.h

// ...

// 사용
#define DeclareReadSetting(Type, Name) extern Type g##Name; \
inline const Type &c##Name() { \
	return g##Name; \
}

// 선언
#define DeclareSetting(Type, Name) DeclareReadSetting(Type, Name) \
inline void cSet##Name(const Type &Name) { \
	g##Name = Name; \
}

// ...

DeclareSetting(int, ScreenScale);
```

```cpp
// 이렇게 쓰자

// ...

// 호출
setScale(
    (scale == cScreenScale()) ? style::kScaleAuto : scale,
    setScale);

// ...

// 선언
cSetScreenScale(80);
```

### .style파일(코드 자동화) 활용법?

😺 `codegen_style.exe`로 부터 .style에서 .cpp로 뽑아낸다.<br>
😺 style파일은 아래와 같이 구성되어 있다.<br>

```
// basic.style

TextPalette {
	linkFg: color;
	monoFg: color;
	selectBg: color;
	selectFg: color;
	selectLinkFg: color;
	selectMonoFg: color;
	selectOverlay: color;
}

TextStyle {
	font: font;
	linkFont: font;
	linkFontOver: font;
	lineHeight: pixels;
}

fsize: 13px;
normalFont: font(fsize);
semiboldFont: font(fsize semibold);
boxFontSize: 14px;
boxTextFont: font(boxFontSize);

// ...
```

😺 별도로 분석을 하지 않는 이유는 python으로 더 간단히 구현이 가능하고 이후에 필요할 경우 👉 `codegen_style`프로젝트를 분석해 보자.<br>
😺 이건 확실히 python으로 코드 자동화하는게 훨 씬 더 편함(**python jinja2**)

---

### 무한 스크롤의 구현

🐳 아래 그림 처럼 채팅이 있다면 무한으로 scrolling이 가능한데 어떻게 구현이 됐을까?

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/qt/telegram-1.gif"/>
</p>