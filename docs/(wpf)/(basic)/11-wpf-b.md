---
layout: default
title: "11. Control"
parent: "(WPF basic)"
grand_parent: "(WPF)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/WPF-Example/tree/master/16.Cotrol/CONTROLBASIC)

* Content안에 Content를 넣을수 있음

```xml
<!-- -->    
    
    <StackPanel>
        <Button Background="Yellow"  Content="ClickMe1" FontSize="20"></Button>
        
        <Button Content="ClickMe2" FontSize="20">
            <Button.Background>
                <SolidColorBrush Color="Red"/>
            </Button.Background>
        </Button>
        
        
        <Button Content="ClickMe3" FontSize="20">
            <Button.Background>
                <LinearGradientBrush EndPoint="0.5,1" StartPoint="0.5,0">
                    <GradientStop Color="Black" Offset="0"/>
                    <GradientStop Color="#FFEDDADA" Offset="1"/>
                    <GradientStop Color="#FF040404" Offset="0.491"/>
                </LinearGradientBrush>
            </Button.Background>
        </Button>
        
        <Button  FontSize="20">
            <StackPanel>
                <CheckBox Content="동의합니다."/>
            </StackPanel>
        </Button>
        
    </StackPanel>
</Window>
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/wpf/basic/b11-1.png"/>
</p>

---

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/wpf/basic/b11-2.png"/>
</p>

```xml
<!-- -->
    
    <StackPanel Orientation="Horizontal">
        
        <Button Content="ClickMe" Width="100"/>
        
        <ListBox Width="100">
            <ListBox.Items>
                <Rectangle Fill="Red" Width="30" Height="30"/>
                <Rectangle Fill="Blue" Width="30" Height="30"/>
            </ListBox.Items>
        </ListBox>
        
        <GroupBox Header="Color" Width="100">
            <StackPanel>
                <RadioButton Content="Red"/>
                <RadioButton Content="Blue"/>
            </StackPanel>
        </GroupBox>

    </StackPanel>
</Window>

```
---

* 애니메이션 넣어보기

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/wpf/basic/b11-3.png"/>
</p>

```xml
<!-- -->

    <Window.Resources>

        <BeginStoryboard x:Key="mystoryboard">
            <Storyboard>
                <DoubleAnimation Storyboard.TargetName ="button1"
                                                 Storyboard.TargetProperty="Width"
                                                 From="100" To="200" Duration="0:0:5"/>
            </Storyboard>
        </BeginStoryboard>
        
    </Window.Resources>
    
    
    
    <StackPanel>        
        
        <Button Click="button1_Click" x:Name="button1"  Content="ClickMe1" FontSize="20"></Button>
        
        <Button Click="Clicked" Content="ClickMe2" FontSize="20">
            <x:Code>
                <![CDATA[
                void Clicked(object sender, RoutedEventArgs e)
                {
                    button1.Content = "Hello World";
                }
                ]]>
            </x:Code>            
        </Button>


        <Button Content="ClickMe3" FontSize="20">
            <Button.Triggers>
                <EventTrigger RoutedEvent="Button.Click">
                    <EventTrigger.Actions>
                        
                        <BeginStoryboard>
                            <Storyboard>
                                <DoubleAnimation Storyboard.TargetName ="button1"
                                                 Storyboard.TargetProperty="Width"
                                                 From="100" To="200" Duration="0:0:5"/>
                            </Storyboard>
                        </BeginStoryboard>

                    </EventTrigger.Actions>
                </EventTrigger>
            </Button.Triggers>
            
        </Button>


        <Button Content="ClickMe4" FontSize="20">

            <Button.Triggers>
                <EventTrigger RoutedEvent="Button.Click">

                    <StaticResource ResourceKey="mystoryboard"/>

                </EventTrigger>
            </Button.Triggers>
        </Button>

    </StackPanel>
</Window>

```