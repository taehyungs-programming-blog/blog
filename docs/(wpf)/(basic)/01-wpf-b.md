---
layout: default
title: "01. 프로젝트 설정"
parent: "(WPF basic)"
grand_parent: "(WPF)"
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

