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

