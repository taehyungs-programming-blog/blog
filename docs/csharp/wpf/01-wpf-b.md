---
layout: default
title: "01. 프로젝트 설정"
parent: "(WPF)"
grand_parent: "(C#)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 환경설정

* Visual Studio 2022
* WPF 어플리케이션
* .NET Framework 빈 프로젝트
* .NET Framework 4.7.2
* 참조 -> 참조 추가 -> 참조 관리자
    * PresentationCore
    * PresentationFramework
    * System.Xaml
    * WindowsBase

* [Get Code 🌎](https://github.com/Arthur880708/WPF-Example/tree/master/01)

---

## 윈도우 띄워보기

* [Get Code 🌎](https://github.com/Arthur880708/WPF-Example/tree/master/02)

```csharp
using System;
using System.Windows;

public class Program
{
    // 우선 [STAThread]는 받아들이자.(꼭 필요함)
    [STAThread]
    public static void Main()
    {
        Application app = new Application();

        Window win = new Window();
        //win.Show();
        //app.MainWindow = win;
        //app.Run();

        // 위와 동일
        app.Run(win);
    }
}
```

* 윈도우 옵션넣기

```csharp
using System;
using System.Windows;
using System.Windows.Media;

public class Program
{
    [STAThread]
    public static void Main()
    {
        Application app = new Application();

        Window win = new Window();

        win.Width = 400;
        win.Height = 400;
        win.Title = "EX12";
        win.Topmost = true;
        win.Background = new SolidColorBrush(Colors.Green);

        app.Run(win);
    }
}
```

---

## STAThread

* **한 줄 정리** - 결국 하고자 하는게 WinForm(UI) 구현이기에 Single Thread를 써야하는데 그걸 선언함.

* 이 속성은 "Single-Threaded Apartment" 모델을 나타내며, 주로 **Windows 기반의 응용 프로그램**에서 사용됩니다. 
* **COM 인터페이스와의 호환성**: Windows의 많은 기존 구성 요소들, 특히 사용자 인터페이스와 관련된 부분들(예: 파일 대화 상자, OLE 드래그 앤 드롭 등)은 COM(Component Object Model) 기술을 기반으로 합니다. 
    * COM 객체들은 두 가지 스레드 모델, STA와 MTA(Multi-Threaded Apartment)를 사용합니다. 
    * STAThread는 이러한 COM 구성 요소들과의 상호 작용이 STA 모드에서 이루어짐을 나타냅니다.
* **단일 스레드 환경**: STA 모델에서는 해당 스레드가 COM 객체의 메시지를 직접 처리합니다. 이는 해당 스레드가 UI 요소와 상호 작용하는 데 중요한 역할을 합니다. 예를 들어, WPF(Windows Presentation Foundation)나 Windows Forms와 같은 UI 프레임워크는 내부적으로 STA 모델을 요구합니다.
* **안정성과 단순성**: STA 모델은 객체에 대한 액세스가 단일 스레드에서만 발생하도록 함으로써 복잡한 동시성 문제를 줄입니다. 이는 UI 구성 요소들의 스레드 안전성을 간소화하고, 애플리케이션의 안정성을 증가시킵니다.
* **요약** - STAThread 속성은 주로 UI 기반의 Windows 응용 프로그램에서 필수적으로 사용되며, COM 구성 요소와의 호환성, 단일 스레드 처리를 통한 안정성 및 간소화를 위해 사용됩니다. 

---

* STAThread의 역할:
    * **UI 구성 요소와의 상호작용**: 예를 들어, 당신이 C# 프로그램에서 파일 대화 상자를 열고자 한다고 가정해봅시다. 이 대화 상자는 COM 기반으로 작동하며, STA 모드에서 최적으로 동작합니다. STAThread는 당신의 메인 스레드가 STA 모드임을 명시하여, 이런 종류의 **COM 구성 요소들과 원활하게 상호작용**할 수 있도록 합니다.
    * **단일 스레드 처리**: STA 모델에서, COM 객체에 대한 모든 호출은 객체가 처음 생성된 스레드로 마샬링(전달)됩니다. 이는 스레드 간의 복잡한 동기화를 피할 수 있게 해줍니다. 예를 들어, UI 컨트롤은 보통 한 스레드(메인 UI 스레드)에서만 안전하게 액세스될 수 있습니다. STAThread속성은 이런 UI 컨트롤들이 올바르게 동작하도록 보장합니다.

---

###  STA와 MTA

* STA (Single-Threaded Apartment)와 MTA (Multi-Threaded Apartment)는 COM(Component Object Model) 기술에서 사용되는 스레드 모델입니다. 이들은 COM 객체가 스레드 간에 어떻게 상호작용하는지를 정의합니다. 

* **STA (Single-Threaded Apartment)**
    * 스레드 독립성: STA 모델에서는 각 스레드가 자신만의 "아파트"를 가집니다. COM 객체는 생성된 그 스레드 내에서만 직접적으로 메서드 호출을 받을 수 있습니다.
    * 메시지 기반 통신: 다른 스레드에서 STA 객체에 접근하려면, 메시지를 통해 해당 스레드의 메시지 큐로 요청을 전달해야 합니다. 이는 스레드 안전성을 높이지만, 성능은 다소 느려질 수 있습니다.
    * UI 컴포넌트와의 호환성: 대부분의 UI 컴포넌트(예: Windows Forms 컨트롤)는 STA 모델을 기반으로 합니다. 이는 UI 컴포넌트가 단일 스레드에서만 접근되어야 하는 스레드 안전성 때문입니다.

* **MTA (Multi-Threaded Apartment)**
    * 동시성: MTA 모델에서는 여러 스레드가 동시에 COM 객체에 접근할 수 있습니다. 이는 더 높은 동시성과 성능을 가능하게 합니다.
    * 스레드 안전성 요구: MTA 모델에서는 개발자가 스레드 간의 동기화와 데이터 일관성을 관리해야 합니다. 이는 복잡도를 증가시킬 수 있습니다.
    * 서버 사이드 애플리케이션에 적합: MTA는 서버 사이드 애플리케이션에서 흔히 사용됩니다, 여기서 동시에 많은 요청을 처리하는 것이 중요합니다.

* 차이점 
    * **스레드 안전성**: STA는 스레드 안전성을 자동으로 관리하지만, MTA는 개발자가 이를 관리해야 합니다.
    * **동시성**: MTA는 더 높은 동시성을 제공하지만, STA는 단일 스레드에서의 작업에 최적화되어 있습니다.
    * 사용 사례: STA는 주로 UI 구성 요소와의 상호작용에 사용되며, MTA는 서버 사이드 애플리케이션과 같이 고성능의 동시 처리가 필요한 경우에 사용됩니다.
* 각각의 모델은 특정 상황과 요구 사항에 맞게 선택되어야 하며, COM 기반 애플리케이션을 설계할 때 중요한 고려 사항입니다.