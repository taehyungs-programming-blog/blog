---
layout: default
title: "02. Event Handling"
parent: "(WPF)"
grand_parent: "(C#)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Window Event

### method override

* [Get Code 🌎](https://github.com/Arthur880708/WPF-Example/tree/master/03)

```csharp
using System;
using System.Windows;
using System.Windows.Input;

public class MainWindow : System.Windows.Window
{
    protected override void OnMouseDown(MouseButtonEventArgs e)
    {
        base.OnMouseDown(e);

        // this.Title = "Mouse Down";
        Point pt = e.GetPosition(this);
        this.Title = $"{pt.X}, {pt.Y}";
    }
}

public class Program
{
    [STAThread]
    public static void Main()
    {
        Application app = new Application();

        MainWindow win = new MainWindow();

        app.Run(win);
    }
}
```

### Window의 event속성에 함수 등록

```csharp
using System;
using System.Windows;
using System.Windows.Input;

public class MainWindow : System.Windows.Window
{
    protected override void OnMouseDown(MouseButtonEventArgs e)
    {

    }
}

public class Program
{
    [STAThread]
    public static void Main()
    {
        Application app = new Application();

        MainWindow win = new MainWindow();
        win.MouseDown += win_MouseDown;

        app.Run(win);
    }

    private static void win_MouseDown(object sender, MouseButtonEventArgs e)
    {
        Window win = (Window)sender;
        Point pt = e.GetPosition(win);
        win.Title = $"{pt.X}, {pt.Y}";
    }
}
```

```csharp
using System;
using System.Windows;
using System.Windows.Input;

public class MainWindow : System.Windows.Window
{
    public MainWindow()
    {
        this.MouseDown += MainWindow_MouseDown;
    }

    private void MainWindow_MouseDown(object sender, MouseButtonEventArgs e)
    {
        Window win = (Window)sender;
        Point pt = e.GetPosition(win);
        win.Title = $"{pt.X}, {pt.Y}";
    }
}

public class Program
{
    [STAThread]
    public static void Main()
    {
        Application app = new Application();

        MainWindow win = new MainWindow();
        
        app.Run(win);
    }
}
```

---

## App Event

* `Application` - 프로그램의 시작, 종료, 이벤트 처리
* `Window` - GUI 담당, 마우스, 키보드 등 처리

```csharp
using System;
using System.Windows;
using System.Windows.Input;

public class MainWindow : System.Windows.Window
{

}

public class App : System.Windows.Application
{
    protected override void OnStartup(StartupEventArgs e)
    {
        base.OnStartup(e);
        Console.WriteLine("OnStartup");

    }

    protected override void OnExit(ExitEventArgs e)
    {
        base.OnExit(e);
        Console.WriteLine("OnExit");
    }
}

public class Program
{
    [STAThread]
    public static void Main()
    {
        App app = new App();

        MainWindow win = new MainWindow();

        app.Run(win);
    }
}
```

* Main을 App안에 넣어도 된다.

```csharp
using System;
using System.Windows;
using System.Windows.Input;

public class MainWindow : System.Windows.Window
{

}

public class App : System.Windows.Application
{
    protected override void OnStartup(StartupEventArgs e)
    {
        base.OnStartup(e);
        Console.WriteLine("OnStartup");

    }

    protected override void OnExit(ExitEventArgs e)
    {
        base.OnExit(e);
        Console.WriteLine("OnExit");
    }

    [STAThread]
    public static void Main()
    {
        App app = new App();

        MainWindow win = new MainWindow();

        app.Run(win);
    }
}

```

* 실제로 WFP가 이렇게 구성이 되어 있음

```csharp
using System;
using System.Windows;

public class MainWindow : System.Windows.Window
{

}

public class App : System.Windows.Application
{
    public App()
    {
        this.Startup += App_Startup;
    }

    private void App_Startup(object sender, StartupEventArgs e)
    {
        Console.WriteLine("OnStartup");
    }

    [STAThread]
    public static void Main()
    {
        App app = new App();

        MainWindow win = new MainWindow();

        app.Run(win);
    }
}

```

---

## Object Reference

* App와 Window객체의 참조를 얻어보자.

```csharp
using System;
using System.Windows;
using System.Windows.Input;

public class MainWindow : System.Windows.Window
{
    protected override void OnMouseDown(MouseButtonEventArgs e)
    {
        base.OnMouseDown(e);

        // main안에서 app을 쓰려면?
        Application.Current.Shutdown();
        ((App)Application.Current).Fn();
    }
}

public class App : System.Windows.Application
{
    public void Fn()
    {
        Console.WriteLine("Fn");
    }

    [STAThread]
    public static void Main()
    {
        App app = new App();

        MainWindow win = new MainWindow();

        app.Run(win);
    }
}
```

```csharp
using System;
using System.Windows;
using System.Windows.Input;

public class MainWindow : System.Windows.Window
{
    public void Fn()
    {
        Console.WriteLine("MainWindow Fn");
    }
}

public class App : System.Windows.Application
{
    protected override void OnStartup(StartupEventArgs e)
    {
        base.OnStartup(e);

        ((MainWindow)MainWindow).Fn();
    }

    protected override void OnExit(ExitEventArgs e)
    {
        base.OnExit(e);

        // crash!! - Main은 이미 소멸됨
        ((MainWindow)MainWindow).Fn();
    }

    [STAThread]
    public static void Main()
    {
        App app = new App();

        MainWindow win = new MainWindow();

        app.Run(win);
    }
}
```