---
layout: default
title: "13. Element Binding"
parent: "(WPF basic)"
grand_parent: "(WPF)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/WPF-Example/tree/master/17.Binding_MVVM/ELEMENTBINDING)

```csharp
public partial class Ex1Window : Window
{
    public Ex1Window()
    {
        InitializeComponent();
    }

    private void button_Click(object sender, RoutedEventArgs e)
    {
        label.FontSize = 30;
        slider.Value = 30;
    }

    private void slider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
    {
        label.FontSize = slider.Value;
    }
}
```

```xml
<!-- -->

    <StackPanel>
        <Label  Name="label" Content="Label" FontSize="30"/>
        <Slider Name="slider" Minimum="10" Maximum="100" Value="30"
                ValueChanged="slider_ValueChanged"/>
        <Button Name="button" Content="ClickMe" Click="button_Click"/>
    </StackPanel>
</Window>
```

---

```csharp
public partial class Ex2Window : Window
{
    public Ex2Window()
    {
        InitializeComponent();

        Binding b = new Binding();
        b.Source = slider;
        b.Path = new PropertyPath("Value"); // slider의 value값을 연결
        b.Mode = BindingMode.TwoWay;   

        // font size를 slider의 value와 binding
        label.SetBinding(System.Windows.Controls.Label.FontSizeProperty, b);
    }

    private void button_Click(object sender, RoutedEventArgs e)
    {
        label.FontSize = 30;
    }
}
```

---

* 예상하듯 TwoWay Bind를 xaml에서 가능

```xml
<!-- -->

    <StackPanel>
        <Label Name="label" Content="Label" 
               FontSize="{Binding ElementName=slider, Path=Value, Mode=TwoWay}"/>  
        
        <Slider Name="slider" Minimum="10" Maximum="100" Value="30"/>
        <Button Name="button" Content="ClickMe" Click="button_Click"/>
    </StackPanel>
</Window>

```