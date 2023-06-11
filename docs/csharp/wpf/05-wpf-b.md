---
layout: default
title: "05. WPF Project 생성, XAML + Code"
parent: "(WPF)"
grand_parent: "(C#)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/WPF-Example/tree/master/06.xaml_code_1)

## WPF Project 생성

* 헷갈리는 점
    * WPF 앱(.NET Framework) - 
        * .NetFramework 기반
        * Windows OS 환경에서만 사용되는 어플개발용
        * 4.8 까지 제공
    * WPF 애플리케이션 -
        * .NET Core 기반
        * 크로스 플랫폼 지원

* WPF 앱 (.NET Framework)로 만들예정

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/wpf/basic/b5-1.png"/>
</p>

* 프로젝트를 생성해 보면 구성이 아래와 같은데
    * **App.xaml** - 프로젝트의 리소스 관리
    * **MainWindow.xaml** - 메인윈도우 관리
    * **MainWindow.xaml.cs** - 메인윈도우의 코드 관리
    * 숨겨진 파일인 **MainWindow.g.i.cs** - xaml과 cs파일의 연결을 담당한다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/wpf/basic/b5-2.png"/>
</p>

```csharp
namespace Ex1
{
    /// <summary>
    /// MainWindow.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            // 여기서 xaml과 cs를 연결한다
            InitializeComponent();
        }
    }
}
```

---

## Event 처리해보기

```csharp
namespace Ex1
{
    /// <summary>
    /// MainWindow.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

            this.MouseDown += MainWindow_MouseDown;
        }

        private void MainWindow_MouseDown(object sender, MouseButtonEventArgs e)
        {
            MessageBox.Show("Hello, World!");
        }
    }
}
```

---

## XAML로 Event처리

```xml
<Window x:Class="Ex1.MainWindow"
        xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
        xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
        xmlns:d="http://schemas.microsoft.com/expression/blend/2008"
        xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
        xmlns:local="clr-namespace:Ex1"
        mc:Ignorable="d"
        Title="MainWindow" Height="450" Width="800"
        MouseDown="MainWindow_MouseDown">
        <!-- Event 등록 -->
    <Grid>
        <Grid.ColumnDefinitions>
            <ColumnDefinition Width="79*"/>
            <ColumnDefinition Width="721*"/>
        </Grid.ColumnDefinitions>

    </Grid>
</Window>
```

---

* [Get Code 🌎](https://github.com/Arthur880708/WPF-Example/tree/master/07.xaml_code_2)

## Grid 나누기

* xaml gui 제어페이지의 상단을 누른다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/wpf/basic/b5-3.png"/>
</p>

* 위와 같은 마킹이 생기고 코드는 아래와 같이 변경된다.

```xml
<Window x:Class="Ex1.MainWindow"
        xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
        xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
        xmlns:d="http://schemas.microsoft.com/expression/blend/2008"
        xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
        xmlns:local="clr-namespace:Ex1"
        mc:Ignorable="d"
        Title="MainWindow" Height="450" Width="800"
        MouseDown="MainWindow_MouseDown">
    <Grid>
        <Grid.ColumnDefinitions>
        <!-- 
            이런게 추가되는데 해석하자면
            1번째 열의 너비는 79로 설정하고
            2번째 열의 너비는 202*로 설정하고
            *가 붙은곳은 비율로 설정된다.
         -->
            <ColumnDefinition Width="79"/>
            <ColumnDefinition Width="202*"/>
            <ColumnDefinition Width="228*"/>
            <ColumnDefinition Width="291*"/>
        </Grid.ColumnDefinitions>

    </Grid>
</Window>
```

---

## Button 넣어보기

* 도구상자 -> Button -> 드래그 앤 드롭

```xml
<Window x:Class="Ex1.MainWindow"
        xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
        xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
        xmlns:d="http://schemas.microsoft.com/expression/blend/2008"
        xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
        xmlns:local="clr-namespace:Ex1"
        mc:Ignorable="d"
        Title="MainWindow" Height="450" Width="800"
        MouseDown="MainWindow_MouseDown">
    <Grid>
        <Grid.ColumnDefinitions>
            <ColumnDefinition Width="79*"/>
            <ColumnDefinition Width="202*"/>
            <ColumnDefinition Width="228*"/>
        </Grid.ColumnDefinitions>
        <Button Grid.Row="0" Grid.Column="0">확인1</Button>
        <Button Grid.Row="0" Grid.Column="1">확인2</Button>
        <Button Grid.Row="0" Grid.Column="2">확인3</Button>
    </Grid>
</Window>
```

---

## Button Event 처리

```xml
<!-- ... -->
<Button Grid.Row="0" Grid.Column="0" Click="Button_Click">확인1</Button>
<Button Grid.Row="0" Grid.Column="1">확인2</Button>
<Button Grid.Row="0" Grid.Column="2">확인3</Button>
<!-- ... -->
```

---

## Property에 이름을 지정할 수 있다.

```xml
<!-- ... -->
    <Grid Name="grid">
        <Grid.ColumnDefinitions>
            <ColumnDefinition Width="79*"/>
            <ColumnDefinition Width="202*"/>
            <ColumnDefinition Width="228*"/>
        </Grid.ColumnDefinitions>
        <Button Name="button1" Grid.Row="0" Grid.Column="0" Click="Button_Click">확인1</Button>
        <Button x:Name="button2" Grid.Row="0" Grid.Column="1">확인2</Button>
        <Button Grid.Row="0" Grid.Column="2">확인3</Button>
    </Grid>
</Window>

```

```csharp
public partial class MainWindow : Window
{
    public MainWindow()
    {
        InitializeComponent();

        // grid를 지정한 적이없는데 ?
        // 이름으로 접근한 케이스
        grid.RowDefinitions.Add(new RowDefinition());
        grid.RowDefinitions.Add(new RowDefinition());

        // 이벤트 처리도 가능하다
        button2.Click += Button_Click;
    }
```

---

## 새 창을 추가하고싶다면 ?

* [Get Code 🌎](https://github.com/Arthur880708/WPF-Example/tree/master/08.xaml_code_3)

* 프로젝트 우클릭 -> 새 항목 -> 창(WPF)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/wpf/basic/b5-4.png"/>
</p>

```csharp
namespace Ex1
{
    /// <summary>
    /// MainWindow.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private void Window_MouseDown(object sender, MouseButtonEventArgs e)
        {
            // 이런식으로 띄울수 있다.
            NewWindow win = new NewWindow();
            win.Show();
        }
    }
}
```

* 시작 xaml은 어디서 설정하나?
    * App.xaml

```xml
<Application x:Class="Ex1.App"
             xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
             xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
             xmlns:local="clr-namespace:Ex1"
             StartupUri="MainWindow.xaml">
             <!-- StartupUri 확인 -->
    <Application.Resources>
         
    </Application.Resources>
</Application>
```
