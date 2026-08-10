---
layout: default
title: "06. XAML Basic"
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

## 준비사항

* .xaml 4개를 추가하자
* 시작 .xaml을 Ex1Window.xaml로 변경하자

```xml
<Application x:Class="xaml.App"
             xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
             xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
             xmlns:local="clr-namespace:xaml"
             StartupUri="Ex1Window.xaml">
    <Application.Resources>
         
    </Application.Resources>
</Application>

```

---

## XAML Basic

* [Get Code 🌎](https://github.com/Arthur880708/WPF-Example/tree/master/09.xaml_basic)

### Background

```xml
<Window x:Class="xaml.Ex1Window"
        xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
        xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
        xmlns:d="http://schemas.microsoft.com/expression/blend/2008"
        xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
        xmlns:local="clr-namespace:xaml"
        mc:Ignorable="d"
        Name="win">
    <!--  Background="Yellow" tag로 넣을수 있음 -->
    <Window.Background>
        <LinearGradientBrush StartPoint="0, 0.5" EndPoint="1, 0.5">
            <GradientStop Color="Red" Offset="0" />
            <GradientStop Color="Yellow" Offset="0.5" />
            <GradientStop Color="Green" Offset="1" />
        </LinearGradientBrush>
    </Window.Background>
</Window>
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/wpf/basic/b6-1.png"/>
</p>

### Content

```xml
<Window x:Class="xaml.Ex2Window"
        xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
        xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
        xmlns:d="http://schemas.microsoft.com/expression/blend/2008"
        xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
        xmlns:local="clr-namespace:xaml"
        mc:Ignorable="d"
        Title="Ex2Window" Height="450" Width="800">
    <!-- Content="Hello" tag에 넣을수 있음 -->
    <Window.Content>Hello</Window.Content>

    <Button Content = "Ok"/>
</Window>
```

### StackPanel

```xml
<Window x:Class="xaml.Ex3Window"
        xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
        xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
        xmlns:d="http://schemas.microsoft.com/expression/blend/2008"
        xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
        xmlns:local="clr-namespace:xaml"
        mc:Ignorable="d"
        Title="Ex3Window" Height="450" Width="800">
    
    <?ignore
    <Window.Content>
        <StackPanel>
            <StackPanel.Children>
                <Button Content="Ok1"/>
                <Button Content="Ok2"/>
            </StackPanel.Children>
        </StackPanel>
    </Window.Content>
    ?>
    
    <!-- 아래와 같이 생략가능 -->

    <StackPanel>
        <Button Content="Ok1"/>
        <Button Content="Ok2"/>
    </StackPanel>
</Window>
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/wpf/basic/b6-2.png"/>
</p>

---

## XAML Namespace

* [Get Code 🌎](https://github.com/Arthur880708/WPF-Example/tree/master/10.xaml_namespace)

* XAML에서 C# Core의 Object인 String을 사용하고 싶다면?
    * `xmlns:System="clr-namespace:System;assembly=mscorlib"` 네임스페이스 추가
    * `System:String`으로 사용

```xml
<Window x:Class="xaml_namespace.MainWindow"
        xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
        xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
        xmlns:d="http://schemas.microsoft.com/expression/blend/2008"
        xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
        xmlns:local="clr-namespace:xaml_namespace"
        xmlns:System="clr-namespace:System;assembly=mscorlib"
        mc:Ignorable="d"
        Title="MainWindow" Height="450" Width="800">
    <ListBox>
        <ListBox.Items>
            <Button>ok</Button>
            <Label>label</Label>
            <System:String>text</System:String>
        </ListBox.Items>
    </ListBox>
</Window>
```

---

## MarkupExtension

* [Get Code 🌎](https://github.com/Arthur880708/WPF-Example/tree/master/11.xaml_markup)

```xml
<Window x:Class="xaml_markup.MainWindow"
        xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
        xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
        xmlns:d="http://schemas.microsoft.com/expression/blend/2008"
        xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
        xmlns:local="clr-namespace:xaml_markup"
        mc:Ignorable="d"
        Title="MainWindow" Height="450" Width="800">
    <Button Content="Hello" Height="100" Width="100"
            Background="{x:Null}"/>
        <!-- 이건 무슨 표기법일까? -->
</Window>
```

* `x:Null`은 x의 이름 공간안에 있는 Null 타입의 객체를 생성 
* 약속된 메소드인 ProvideValue를 호출하여 객체를 생성
* `{}` 안에 들어가는 타입은 반드시 MarkupExtension을 상속받아야 함

```xml
<Button Content="Hello" Height="100" Width="100"
        Background="{local:Null}"/>
```

* 우리가 만든 Null 객체를 써보겠다는 말
    * 이제 Null 객체를 만들어보자

```csharp
namespace xaml_markup
{
    public class Null : MarkupExtension
    {
        public override object ProvideValue(IServiceProvider serviceProvider)
        {
            // return null; // 투명
            return new SolidColorBrush(Colors.Yellow);
        }
    }

    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }
    }
}
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/wpf/basic/b6-3.png"/>
</p>

---

## Markup 추가1

* 아래와 같은 markup이 있다.

```csharp
namespace xaml_markup
{
    public class Header : MarkupExtension
    {
        private string key;

        public Header(string s) { key = s; }

        public override object ProvideValue(IServiceProvider serviceProvider)
        {
            switch(key)
            {
                case "FontSize": return (Double)30;
                case "Background": return new SolidColorBrush(Colors.Yellow);
            }
            return null;
        }
    }

    public partial class Markup2 : Window
    {
        public Markup2()
        {
            InitializeComponent();
        }
    }
}
```

* 어떻게 xaml에서 사용할까?

```xml
<Window x:Class="xaml_markup.Markup2"
        xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
        xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
        xmlns:d="http://schemas.microsoft.com/expression/blend/2008"
        xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
        xmlns:local="clr-namespace:xaml_markup"
        mc:Ignorable="d"
        Title="Markup2" Height="450" Width="800">
    <Button Content="Hello" Height="450" Width="800"
            Background="{local:Header Background}"
            FontSize="{local:Header FontSize}"
            />
</Window>
```

---

## Markup 추가2

* 아래와 같은 markup이 있다.

```csharp
namespace xaml_markup
{
    public class FontInfo : MarkupExtension
    {
        public string Element { get; set; }
        public string key { get; set; }

        public override object ProvideValue(IServiceProvider serviceProvider)
        {
            if(Element == "Title")
            {
                switch(key)
                {
                    case "FontSize": return (Double)30;
                    case "Background": return new SolidColorBrush(Colors.Yellow);
                }
            }
            return null;
        }
    }

    public partial class Markup2 : Window
    {
        public Markup2()
        {
            InitializeComponent();
        }
    }
}
```

```xml
<Window x:Class="xaml_markup.Markup2"
        xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
        xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
        xmlns:d="http://schemas.microsoft.com/expression/blend/2008"
        xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
        xmlns:local="clr-namespace:xaml_markup"
        mc:Ignorable="d"
        Title="Markup2" Height="450" Width="800">
    <Button Content="Hello" Height="450" Width="800"
            Background="{local:FontInfo Element=Title, key=Background}"
            />
</Window>
```