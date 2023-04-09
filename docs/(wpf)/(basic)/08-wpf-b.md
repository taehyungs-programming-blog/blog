---
layout: default
title: "08. Layout"
parent: "(WPF basic)"
grand_parent: "(WPF)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/WPF-Example/tree/master/13.layout/13.1.canvas)

## CANVAS

```xml
<Window x:Class="CANVAS.MainWindow"
        xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
        xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
        xmlns:d="http://schemas.microsoft.com/expression/blend/2008"
        xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
        xmlns:local="clr-namespace:CANVAS"
        mc:Ignorable="d"
        Title="MainWindow" Height="450" Width="800">
    <Canvas Name="canvas">
        <Button Canvas.Top="100" Canvas.Left="30">button1</Button>
        <Button Canvas.Top="150" Canvas.Left="30">button2</Button>
        <Button Canvas.Top="200" Canvas.Left="30">button3</Button>

        <Ellipse Panel.ZIndex="1"   Canvas.Top="150" Canvas.Left="200" Width="100" Height="100" Fill="Blue"></Ellipse>
        <Rectangle Panel.ZIndex="2" Canvas.Top="200" Canvas.Left="200" Width="100" Height="100" Fill="Red"></Rectangle>

    </Canvas>
</Window>
```

```csharp
public partial class MainWindow : Window
{
    public MainWindow()
    {
        InitializeComponent();

        //Canvas v = new Canvas();

        Button btn = new Button();
        btn.Content = "확인";
        btn.Width = 100;

        Canvas.SetTop(btn, 30);
        Canvas.SetLeft(btn, 30);

        canvas.Children.Add(btn);
    }
}
```

---

## Sketch

* [Get Code 🌎](https://github.com/Arthur880708/WPF-Example/tree/master/13.layout/13.2.sketch)

```xml
<Window x:Class="Ex1_Sketch.MainWindow"
        xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
        xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
        xmlns:d="http://schemas.microsoft.com/expression/blend/2008"
        xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
        xmlns:local="clr-namespace:Ex1_Sketch"
        mc:Ignorable="d"
        Title="Sketch" Height="450" Width="800" Topmost="True" >
    <Canvas Name="canvas" Background="white"  MouseMove="canvas_MouseMove"
                          MouseLeftButtonDown="canvas_MouseLeftButtonDown">
    <!--
        Background="white" 을 넣은 이유는 null일경우 Event를 못받음
     -->
        
    </Canvas>
    
</Window>

```

```csharp
namespace Ex1_Sketch
{
    /// <summary>
    /// MainWindow.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

        //  canvas.Background = new SolidColorBrush(Colors.Yellow);
        }


        private Point ptFrom;

        private void canvas_MouseMove(object sender, MouseEventArgs e)
        {
            if (e.LeftButton == MouseButtonState.Pressed)
            {
                Point ptTo = e.GetPosition(this);

                Line line = new Line();
                line.Stroke = new SolidColorBrush(Colors.Red);

                line.X1 = ptFrom.X;
                line.Y1 = ptFrom.Y;
                line.X2 = ptTo.X;
                line.Y2 = ptTo.Y;

                canvas.Children.Add(line);

                ptFrom = ptTo;
            }
        }

        private void canvas_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            ptFrom = e.GetPosition(this);   
        }
    }
}

```

---

## StackPanel

* [Get Code 🌎](https://github.com/Arthur880708/WPF-Example/tree/master/13.layout/13.4.stackpanel)

* 마치 Stack 처럼 object를 추가한 순서대로 배치

```xml
<Window x:Class="STACKPANEL.MainWindow"
        xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
        xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
        xmlns:d="http://schemas.microsoft.com/expression/blend/2008"
        xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
        xmlns:local="clr-namespace:STACKPANEL"
        mc:Ignorable="d"
        Title="MainWindow" Height="200" Width="400">
    <StackPanel Margin="20" Orientation="Horizontal">
        <Button Margin="3">Button1</Button>
        <Button Margin="10,20,30,40">Button2</Button>
        <Button>Button3</Button>
        <CheckBox HorizontalAlignment="Center">동의</CheckBox>

    </StackPanel>
</Window>
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/wpf/basic/b8-1.png"/>
</p>

---

## DockPanel

* [Get Code 🌎](https://github.com/Arthur880708/WPF-Example/tree/master/13.layout/13.3.dockpanel)

* 자식 object를 상하좌우로 배치

```xml
<Window x:Class="DOCKPANEL.MainWindow"
        xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
        xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
        xmlns:d="http://schemas.microsoft.com/expression/blend/2008"
        xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
        xmlns:local="clr-namespace:DOCKPANEL"
        mc:Ignorable="d"
        Title="MainWindow" Height="200" Width="400">
    <DockPanel LastChildFill="False">
        <Button DockPanel.Dock="Top">Button1</Button>
        <Button DockPanel.Dock="Top" HorizontalAlignment="Left" Width="300">Button2</Button>
        <Button DockPanel.Dock="Top" HorizontalAlignment="center">Button3</Button>
        <Button DockPanel.Dock="Left">Button4</Button>
        <Button DockPanel.Dock="Right">Button5</Button>
        <Button DockPanel.Dock="Bottom">Button6</Button>
        <Button VerticalAlignment="center">Button7</Button>
        <Button>Button8</Button>

    </DockPanel>
</Window>
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/wpf/basic/b8-2.png"/>
</p>

---

## Stack, Dock Example

* [Get Code 🌎](https://github.com/Arthur880708/WPF-Example/tree/master/13.layout/13.5.stack.dock.example)

```xml
<Window x:Class="Ex2_Dialog.MainWindow"
        xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
        xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
        xmlns:d="http://schemas.microsoft.com/expression/blend/2008"
        xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
        xmlns:local="clr-namespace:Ex2_Dialog"
        mc:Ignorable="d"
        Title="MainWindow" Height="200" Width="400">
    
    <DockPanel LastChildFill="True">
        <StackPanel DockPanel.Dock="Bottom" Orientation="Horizontal" HorizontalAlignment="Right">
            <Button Margin="5" Padding="5">확인</Button>
            <Button Margin="5" Padding="5">취소</Button>
          
        </StackPanel>

        <TextBox DockPanel.Dock="Top" Margin="10">Dialog</TextBox>
    </DockPanel>
</Window>

```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/wpf/basic/b8-3.png"/>
</p>

---

## Grid

* [Get Code 🌎](https://github.com/Arthur880708/WPF-Example/tree/master/13.layout/13.6.Grid)

```xml
<Window x:Class="GRID.MainWindow"
        xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
        xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
        xmlns:d="http://schemas.microsoft.com/expression/blend/2008"
        xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
        xmlns:local="clr-namespace:GRID"
        mc:Ignorable="d"
        Title="MainWindow" Height="200" Width="400">

    <Grid ShowGridLines="True" Name="grid">
        <Grid.RowDefinitions>
            <RowDefinition Height="50*"/>
            <RowDefinition Height="50*"/>
        </Grid.RowDefinitions>
        <Grid.ColumnDefinitions>
            <ColumnDefinition/>
            <ColumnDefinition/>
            <ColumnDefinition/>
        </Grid.ColumnDefinitions>

        <Button Name="button1" Grid.Row="0" Grid.Column="0" Click="button1_Click" >button1</Button>
        <Button Name="button2" Grid.Row="0" Grid.Column="1" Grid.ColumnSpan="2" Click="button2_Click">button2</Button>
        
        <Button Name="button3" Grid.Row="1" Grid.Column="2" Width="50" Height="30" Margin="29,26,54,36" Panel.ZIndex="1">button3</Button>
        <Button Name="button4" Grid.Row="1" Grid.Column="2" Width="50" Height="30" Margin="66,46,17,16">button4</Button>

    </Grid>
</Window>

```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/wpf/basic/b8-4.png"/>
</p>

### LINQ 표기법

```csharp
int[] arr = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
var result = arr.Where(x => x % 2 == 0);
foreach (var item in result)
{
    Console.WriteLine(item);
}

// or

object[] arr = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
var result = arr.Cast<int>().Where(x => x % 2 == 0);
foreach (var item in result)
{
    Console.WriteLine(item);
}

// 첫 번째 꺼만 찾아보자
var result = arr.Cast<int>().FirstOrDefault(x => x % 2 == 0);
```

---

## GridSplitter

* [Get Code 🌎](https://github.com/Arthur880708/WPF-Example/tree/master/13.layout/13.7.GridSplitter)

```xml
<Window x:Class="GRIDSPLITTER.MainWindow"
        xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
        xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
        xmlns:d="http://schemas.microsoft.com/expression/blend/2008"
        xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
        xmlns:local="clr-namespace:GRIDSPLITTER"
        mc:Ignorable="d"
        Title="MainWindow" Height="200" Width="400">
    <Grid>
        <Grid.RowDefinitions>
            <RowDefinition />
            <RowDefinition Height="auto"/>
            <RowDefinition />
        </Grid.RowDefinitions>
        <Grid.ColumnDefinitions>
            <ColumnDefinition />
            <ColumnDefinition Width="auto"/>
            <ColumnDefinition />
        </Grid.ColumnDefinitions>

        <Button Grid.Row="0" Grid.Column="0">button1</Button>
        <Button Grid.Row="0" Grid.Column="2">button2</Button>
        <Button Grid.Row="2" Grid.Column="0">button3</Button>

        <GridSplitter Grid.Row="0" Grid.Column="1" Grid.RowSpan="3" Width="5" HorizontalAlignment="Center" VerticalAlignment="Stretch"/>
        <GridSplitter Grid.Row="1" Grid.Column="0" Grid.ColumnSpan="3" Height="5" VerticalAlignment="Center" HorizontalAlignment="Stretch"/>

    </Grid>
</Window>
``` 

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/wpf/basic/b8-5.png"/>
</p>
