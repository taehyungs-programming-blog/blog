---
layout: default
title: "16. Menu"
parent: "(WPF basic)"
grand_parent: "(WPF)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/WPF-Example/tree/master/19.Menu)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/wpf/basic/b16-1.png"/>
</p>

```xml
<Window x:Class="MENU.MainWindow"
        xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
        xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
        xmlns:d="http://schemas.microsoft.com/expression/blend/2008"
        xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
        xmlns:local="clr-namespace:MENU"
        mc:Ignorable="d"
        Title="MainWindow" Height="200" Width="300">

    <Window.Resources>
        <RoutedCommand x:Key="cmdAction" />
    </Window.Resources>


    <Window.CommandBindings>
        <CommandBinding Command="{StaticResource ResourceKey=cmdAction}" Executed="MenuItem_Click"/>
    </Window.CommandBindings>

    <Window.InputBindings>
        <KeyBinding Key="O" Modifiers="Control" Command="{StaticResource ResourceKey=cmdAction}"/>
    </Window.InputBindings>


    <StackPanel>
        <Menu>
            <MenuItem Header="File">
                <MenuItem Header="One" InputGestureText="Ctrl+O" Command="{StaticResource ResourceKey=cmdAction}">
                    <MenuItem.Icon>
                        <Image Source="images/melon.png" />
                    </MenuItem.Icon>
                </MenuItem>
                
                <MenuItem Header="Two" Click="MenuItem_Click"/>
                <Separator/>
                <MenuItem Header="Exit"/>
            </MenuItem>
            
            <MenuItem Header="Help">
                <MenuItem Header="Three"/>
                <MenuItem Header="Four"/>                
            </MenuItem>
        </Menu>

        
        
        <ToolBarTray >
            <ToolBar >
                <Button  Command="{StaticResource ResourceKey=cmdAction}">
                    <StackPanel>
                        <Image Source="images/melon.png" />
                        <Label>OK</Label>
                    </StackPanel>
                </Button>
                <Button>Two</Button>
                <Button>Three</Button>
            </ToolBar>
        </ToolBarTray >
        
    </StackPanel>
</Window>

```