---
layout: default
title: "07. Sliding Puzzle Game 만들어보기"
parent: "(WPF)"
grand_parent: "(C#)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/WPF-Example/tree/master/12.puzzle_game)

* 아래와 같은 totoro puzzle game을 만들어보자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/wpf/basic/b7-1.png"/>
</p>

```xml
<Window x:Class="SlidingPuzzle.MainWindow"
        xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
        xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
        xmlns:d="http://schemas.microsoft.com/expression/blend/2008"
        xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
        xmlns:local="clr-namespace:SlidingPuzzle"
        mc:Ignorable="d"
        Title="MainWindow" Height="450" Width="800">
    <Grid>

    <!-- 각 Row 생성 -->
        <Grid.RowDefinitions>
            <RowDefinition Height="50"/>
            <RowDefinition Height="50"/>
            <RowDefinition />
        </Grid.RowDefinitions>

    <!-- Row에 들어갈 Text와 Button선언 -->
        <TextBlock Grid.Row="0" FontSize="30" HorizontalAlignment="Center" VerticalAlignment="Center">Sliding Puzzlie</TextBlock>
        <Button Grid.Row="1" Width="100" Height="30">Shuffle</Button>

    <!-- Row에 들어갈 Grid 생성, C#코드에 gameGrid 참조 -->
        <Grid Grid.Row="2" Name="gameGrid"> </Grid>
        
    </Grid>
</Window>
```

```csharp
// gameGrid에 Row와 Column을 추가하는 코드
public void InitGameGrid()
{
    for (int i = 0; i < COUNT; i++)
    {
        gameGrid.RowDefinitions.Add(new RowDefinition());
        gameGrid.ColumnDefinitions.Add(new ColumnDefinition());
    }
}
```

* 이미지 그리기

```csharp
public void DrawGameGrid()
{
    BitmapImage bitmap = new BitmapImage();
    bitmap.BeginInit();
    bitmap.UriSource = new Uri("D:\\totoro.jpg");
    bitmap.EndInit();

    width = bitmap.Width / COUNT;
    height = bitmap.Height / COUNT;

    for (int y = 0; y < COUNT; y++)
    {
        for (int x = 0; x < COUNT; x++)
        {
            if (board[y, x] != EMPTY)
            {
                int no = board[y, x];
                int bx = no % COUNT;
                int by = no / COUNT;

                CroppedBitmap cb = new CroppedBitmap(bitmap,
                                    new Int32Rect((int)(bx * width), (int)(by * height),
                                            (int)width, (int)height));

                Image img = new Image();
                //img.Source = bitmap; // 전체 그림.
                img.Source = cb; // 한개 블럭 그림
                img.Stretch = Stretch.Fill;
                img.Margin = new Thickness(0.5);

                Grid.SetRow(img, y);
                Grid.SetColumn(img, x);

                gameGrid.Children.Add(img);
            }
        }

    }
}
```

```csharp
public void SwapBlock(int x1, int y1, int x2, int y2)
{
    // 배열값 교환
    int temp = board[y1, x1];
    board[y1, x1] = board[y2, x2];
    board[y2, x2] = temp;

    // grid내부의 image 교환
    Image img1 = gameGrid.Children.Cast<Image>().FirstOrDefault(n => Grid.GetRow(n) == y1 && Grid.GetColumn(n) == x1);
    Image img2 = gameGrid.Children.Cast<Image>().FirstOrDefault(n => Grid.GetRow(n) == y2 && Grid.GetColumn(n) == x2);

    if (img1 != null)
    {
        Grid.SetRow(img1, y2);
        Grid.SetColumn(img1, x2);
    }

    if (img2 != null)
    {
        Grid.SetRow(img2, y1);
        Grid.SetColumn(img2, x1);
    }
}
```