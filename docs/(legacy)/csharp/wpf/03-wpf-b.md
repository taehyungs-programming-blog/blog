---
layout: default
title: "03. Content Property"
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

* [Get Code 🌎](https://github.com/Arthur880708/WPF-Example/tree/master/04)

## Content Property

* Window는 Content Property에 지정된 객체를 화면에 보여준다.

```csharp
public class MainWindow : System.Windows.Window
{
    public MainWindow()
    {
        this.Content = "Hello";
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
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/wpf/basic/b3-1.png"/>
</p>

### 버튼생성

```csharp
public class MainWindow : System.Windows.Window
{
    public MainWindow()
    {
        Button btn = new Button();
        this.Content = btn;
        btn.Content = "Click";
    }
}
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/wpf/basic/b3-2.png"/>
</p>

### 이미지 읽어오기

```csharp
public class MainWindow : System.Windows.Window
{
    public MainWindow()
    {
        BitmapImage bitmap = new BitmapImage();
        bitmap.BeginInit();
        bitmap.UriSource = new Uri("D:\\sample.png");
        bitmap.EndInit();

        Image img = new Image();
        img.Source = bitmap;
        // img.Stretch = Stretch.Fill; // 가득채우기
        this.Content = img;
    }
}
```

### Layout

* Windows는 자체 Content를 통해 하나의 객체만 연결이 가능하다
    * Layout을 통해 여러개의 객체를 연결할 수 있다.
    * Layout에 다양한 종류가 있는데, 대표적으로 StackPanel, DockPanel, Grid가 있다.
    * XAML을 배운 후 다른걸 살펴볼 예정이고 이번에는 Grid만 사용할 예정

```csharp
public class MainWindow : System.Windows.Window
{
    private Grid grid = null;
    public MainWindow()
    {
        grid = new Grid();

        // row를 두 개
        grid.RowDefinitions.Add(new RowDefinition());
        grid.RowDefinitions.Add(new RowDefinition());

        // column을 두 개
        grid.ColumnDefinitions.Add(new ColumnDefinition());
        grid.ColumnDefinitions.Add(new ColumnDefinition());

        Button btn1 = new Button();
        btn1.Content = "확인1";
        Button btn2 = new Button();
        btn2.Content = "확인2";
        Button btn3 = new Button();
        btn3.Content = "확인3";
        Button btn4 = new Button();
        btn4.Content = "확인4";

        Grid.SetRow(btn1, 0);
        Grid.SetColumn(btn1, 0);

        Grid.SetRow(btn2, 0);
        Grid.SetColumn(btn2, 1);

        Grid.SetRow(btn3, 1);
        Grid.SetColumn(btn3, 0);

        Grid.SetRow(btn4, 1);
        Grid.SetColumn(btn4, 1);

        grid.Children.Add(btn1);
        grid.Children.Add(btn2);
        grid.Children.Add(btn3);
        grid.Children.Add(btn4);

        this.Content = grid;
    }
}
```

### 내부 Content Event등록하기

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/wpf/basic/b3-3.png"/>
</p>

```csharp
    btn1.Click += Btn1_Click;
    btn2.Click += Btn2_Click;
    btn3.Click += Btn3_Click;
    btn4.Click += Btn4_Click;
}

private void Btn2_Click(object sender, RoutedEventArgs e)
{
    Console.WriteLine($"Btn2 Clicked");
}

private void Btn3_Click(object sender, RoutedEventArgs e)
{
    Button btn = sender as Button;
    string s = btn.Content as string;

    Console.WriteLine($"{s} Clicked");
}

private void Btn1_Click(object sender, RoutedEventArgs e)
{
    Console.WriteLine($"Btn1 Clicked");
}
```