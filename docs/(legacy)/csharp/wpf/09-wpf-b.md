---
layout: default
title: "09. Routed Event"
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

## 이론

* Image에 MouseClick Event를 받으려한다.

```csharp
void Image_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
{
    // Do something
}
```

* 이게 맞을까?
    * Image는 Image를 그리는 용도로만 사용하고 싶다

```csharp
void Window_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
{
    // 여기서 처리하고 싶음
}
```

* 어떻게 해야할까? -> Routed Event 사용
* **Routed Event** -> 자녀에게 발생한 Event를 부모까지 전달
* Routed Event는 2가지 방법으로 나뉜다
    * **Bubbling Event** -> event를 발생시킨 객체가 제일 먼저 이벤트를 처리할 권한을 가지게 된다.
    * **Tunneling Event** -> 부모에서 자녀로 전달

---

## ROUTED EVENT

* [Get Code 🌎](https://github.com/Arthur880708/WPF-Example/tree/master/14.RoutedEvent/ROUTEDEVENT)

```xml
<Window x:Class="ROUTEDEVENT.MainWindow"
        xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
        xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
        xmlns:d="http://schemas.microsoft.com/expression/blend/2008"
        xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
        xmlns:local="clr-namespace:ROUTEDEVENT"
        mc:Ignorable="d"
        Title="MainWindow" Height="200" Width="400"  
        MouseLeftButtonDown="Window_MouseLeftButtonDown"
        PreviewMouseLeftButtonDown="Window_PreviewMouseLeftButtonDown"
        >

    <Grid Name="grid"
          MouseLeftButtonDown="Grid_MouseLeftButtonDown"
          PreviewMouseLeftButtonDown="Grid_PreviewMouseLeftButtonDown">

        <Image Source="images/cloud.png" MouseLeftButtonDown="Image_MouseLeftButtonDown"
               PreviewMouseLeftButtonDown="Image_PreviewMouseLeftButtonDown"
               />

    </Grid>
    
</Window>
```

```csharp
namespace ROUTEDEVENT
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
        private void Image_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            Console.WriteLine("Image_MouseLeftButtonDown");
        }

        private void Grid_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            Console.WriteLine("Grid_MouseLeftButtonDown");
            
            //  e.Handled = true;
                // 더 이상 event를 처리하지 마시오!
        }

        private void Window_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            Console.WriteLine("Window_MouseLeftButtonDown");          
        }

        private void Image_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            Console.WriteLine("Image_PreviewMouseLeftButtonDown");
        }

        private void Grid_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            Console.WriteLine("Grid_PreviewMouseLeftButtonDown");

           // e.Handled = true;
        }

        private void Window_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            Console.WriteLine("Window_PreviewMouseLeftButtonDown");
        }
    }
}
```

* 클릭을 할시 아래와 같이 처리된다.

```
// 순서를 보면 Preview는 Window->Grid->Image 순으로 처리되고
    // Tunneling Event라 한다.
Window_PreviewMouseLeftButtonDown
Grid_PreviewMouseLeftButtonDown
Image_PreviewMouseLeftButtonDown

// Bubbling Event라 한다.
Image_MouseLeftButtonDown
Grid_MouseLeftButtonDown
Window_MouseLeftButtonDown
```

* 주의할 점은 BackGround가 Null일 경우 Event를 받을수 없다는 점이다.
    * 꼭 투명을 줘야할 경우 비워두지말고 Transparent로 줘야한다.

---

## 여러 Button의 Event를 하나의 함수에서 처리해 보자

* [Get Code 🌎](https://github.com/Arthur880708/WPF-Example/tree/master/14.RoutedEvent/BUTTONCLICK)

```csharp
public partial class MainWindow : Window
{
    public MainWindow()
    {
        InitializeComponent();
    }

    private void btn_Click(object sender, RoutedEventArgs e)
    {
    //    Button btn = (Button)sender;

        // tag정보로도 구분이 가능
        //   Console.WriteLine($"{btn.Name}, {btn.Content},{btn.Tag}");
    }

    private void StackPanel_Click(object sender, RoutedEventArgs e)
    {
        //Button btn = (Button)sender; // stack panel 자체의 참조

        Button btn = (Button)e.Source;

        Console.WriteLine($"{btn.Name}, {btn.Content},{btn.Tag}");
    }
}
```