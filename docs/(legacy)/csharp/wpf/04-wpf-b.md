---
layout: default
title: "04. XAML"
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

* [Get Code 🌎](https://github.com/Arthur880708/WPF-Example/tree/master/05.xaml)

## xaml 사용

```xml
<Grid xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation">
    <Button>확인</Button>
</Grid>
```

```csharp
public class MainWindow : System.Windows.Window
{
    protected void InitializeComponent()
    {
        Grid grid = null;

        using (FileStream fs = new FileStream("../../ex70.xaml", FileMode.Open))
        {
            grid = (Grid)XamlReader.Load(fs);
        }

        this.Content = grid;
    }
    public MainWindow()
    {
        InitializeComponent();
    }
}

public class App : System.Windows.Application
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

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/wpf/basic/b4-1.png"/>
</p>

---

## Event 연결

```csharp
public class MainWindow : System.Windows.Window
{
    protected void InitializeComponent()
    {
        Grid grid = null;

        using (FileStream fs = new FileStream("../../ex70.xaml", FileMode.Open))
        {
            grid = (Grid)XamlReader.Load(fs);
        }

        this.Content = grid;

        Button btn = (Button)grid.FindName("Button1");
        btn.Click += Btn_Click;
    }

    private void Btn_Click(object sender, RoutedEventArgs e)
    {
        Console.WriteLine("Button 1 Clicked");
    }

    public MainWindow()
    {
        InitializeComponent();
    }
}
```

---

## xaml로도 윈도우를 만들수 있다.

```csharp
public class App : System.Windows.Application
{

    [STAThread]
    public static void Main()
    {
        App app = new App();

        Window win = null;
        using (FileStream fs = new FileStream("../../ex71.xaml", FileMode.Open))
        {
            win = (Window)XamlReader.Load(fs);
        }

        app.Run(win);
    }
}
```

* 윈도우의 이벤트 처리는 어디서 해야할까?

```csharp
public class MainWindow : System.Windows.Window
{
    protected override void OnMouseDown(MouseButtonEventArgs e)
    {
        base.OnMouseDown(e);
        Console.WriteLine("MouseDown");
    }
}

public class App : System.Windows.Application
{

    [STAThread]
    public static void Main()
    {
        App app = new App();

        MainWindow win = null;
        using (FileStream fs = new FileStream("../../ex72.xaml", FileMode.Open))
        {
            win = (MainWindow)XamlReader.Load(fs);
        }

        app.Run(win);
    }
}
```

```xml
<local:MainWindow xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation" 
                  xmlns:local="clr-namespace:;assembly=WPFExamples" 
                  Title="ex72" Width="400" Height="400">
    <Grid>
        <Button Name="button1">확인</Button>
    </Grid>
</local:MainWindow>
```

* `clr-namespace` - namepsace를 의미한다. 현재는 없기에 비워 둠
* `assembly` - 현재 프로젝트의 이름을 넣어준다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/wpf/basic/b4-2.png"/>
</p>