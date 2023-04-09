---
layout: default
title: "10. Resource"
parent: "(WPF basic)"
grand_parent: "(WPF)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Resource

* [Get Code 🌎](https://github.com/Arthur880708/WPF-Example/tree/master/15.Resource_Style/RESOURCE)

* Resource - C# Code와 XAML에서 공통으로 사용하는 변수

```csharp
namespace RESOURCE
{
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

            // 이런식으로 선언가능
            this.Resources["MyBrush"] = new SolidColorBrush(Colors.Yellow);
        }

        private void button4_Click(object sender, RoutedEventArgs e)
        {
            this.Resources["MyBrush"] = new SolidColorBrush(Colors.Red);
        }
    }
}
```

```xml
<!-- -->
    
    <!-- xaml도 역시 이런식으로 사용가능 -->
    <Window.Resources>
        <SolidColorBrush x:Key="MyBrush" Color="Yellow"/>
    </Window.Resources>
    
    
    <StackPanel>
        
        <Button Background="AliceBlue" Content="Button1" FontSize="30"/>
        
        <Button Background="{StaticResource MyBrush}" Content="Button2" FontSize="30"/>

        <Button Background="{DynamicResource MyBrush}" Content="Button3" FontSize="30"/>

        <Button Background="AliceBlue" Content="Button4" FontSize="30" 
                x:Name="button4" Click="button4_Click"/>

    </StackPanel>
</Window>

```

* 보통은 XAML에서 생성함
* **StaticResource** - key값의 객체가 변경되어도 자동으로 변경되지 않음
* **DynamicResource** - key값의 객체가 변경되면 자동으로 변경됨

---

* [Get Code 🌎](https://github.com/Arthur880708/WPF-Example/tree/master/15.Resource_Style/RESOURCE2)

```xml
<!-- -->

    <!-- 다른 데이터도 Resource(object)로 담을수 있다. -->
    <Window.Resources>
        <SolidColorBrush x:Key="MyBrush" Color="Yellow"/>
        <system:String   x:Key="msg">환영합니다</system:String>
        <local:Car x:Key="mycar"/>
        
    </Window.Resources>

    <StackPanel>

        <Button Background="{StaticResource MyBrush}" FontSize="30"
                Content="{StaticResource msg}" />

        <Button Click="Button_Click" Content="ClickMe" FontSize="30"/>

    </StackPanel>
</Window>
```

---

## 리소스를 별도의 파일로 분리해보자.

* [Get Code 🌎](https://github.com/Arthur880708/WPF-Example/tree/master/15.Resource_Style/RESOURCE3)

```xml
<!-- MyResource.xaml -->
<ResourceDictionary xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
                    xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml">

    <SolidColorBrush x:Key="brush3" Color="Blue"/>
    
</ResourceDictionary>
```

```xml
<!-- App.xaml에 등록해야함. -->

<Application x:Class="RESOURCE3.App"
             xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
             xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
             xmlns:local="clr-namespace:RESOURCE3"
             StartupUri="MainWindow.xaml">
    <Application.Resources>
        
        <ResourceDictionary>
            <SolidColorBrush x:Key="brush2" Color="Green"/>
            
            <ResourceDictionary.MergedDictionaries>
                <ResourceDictionary Source="MyResource.xaml"/>
            </ResourceDictionary.MergedDictionaries>
        </ResourceDictionary>
        
    </Application.Resources>
</Application>

```

---

## Style

* [Get Code 🌎](https://github.com/Arthur880708/WPF-Example/tree/master/15.Resource_Style/STYLE1)

```xml
<!-- -->

    <Window.Resources>

        <Style x:Key="bluestyle">
            <Setter Property="Control.Foreground" Value="white"/>
            <Setter Property="Control.Background" Value="blue"/>
            <Setter Property="Control.FontSize" Value="30"/>
        </Style>
        
        
    </Window.Resources>
    
    
    <StackPanel>
        <Button Content="button1" FontSize="30" Background="Red" Foreground="White"/>
        
        <Button x:Name="button2" Content="button2" />
        <Button x:Name="button3" Content="button3" 
                Style="{StaticResource redstyle}"/>

        <Button x:Name="button4" Content="button4" 
                Style="{StaticResource bluestyle}"/>

    </StackPanel>
</Window>

```

```csharp
namespace STYLE1
{

    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            Style style = new Style();

            style.Setters.Add(new Setter(Control.ForegroundProperty,
                                        new SolidColorBrush(Colors.White)));

            style.Setters.Add(new Setter(Control.BackgroundProperty,
                                        new SolidColorBrush(Colors.Red)));

            style.Setters.Add(new Setter(Control.FontSizeProperty,
                            (Double)30));

            this.Resources["redstyle"] = style;
            
            InitializeComponent();

            button2.Style = style;
        }
    }
}

```

---

## Examples

* [Get Code 🌎](https://github.com/Arthur880708/WPF-Example/tree/master/15.Resource_Style/STYLE2)

