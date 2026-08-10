---
layout: default
title: "15. Command"
parent: "(WPF)"
grand_parent: "(C#)"
great_grand_parent: "Legacy Archive"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/WPF-Example/tree/master/18.Command/18.1.Command)

## text를 입력받으면 button을 활성화 해보자.

* 기존에 사용하던 방식

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/wpf/basic/b15-1.png"/>
</p>

```xml
<!-- -->

    <StackPanel>
        <TextBox TextChanged="txtBox_TextChanged" x:Name="txtBox" Margin="10" FontSize="30"/>
        <Button  Click="button_Click"             x:Name="button" Margin="10" FontSize="30" Content="button1"/>
    </StackPanel>
</Window>
```

```csharp
public partial class Command1Window : Window
{
    public Command1Window()
    {
        InitializeComponent();
        button.IsEnabled = !string.IsNullOrEmpty(txtBox.Text);
    }

    private void txtBox_TextChanged(object sender, TextChangedEventArgs e)
    {
        button.IsEnabled = !string.IsNullOrEmpty(txtBox.Text);   
    }

    private void button_Click(object sender, RoutedEventArgs e)
    {
        MessageBox.Show(txtBox.Text);
    }
}
```

---

## Command를 활용해 보자.

```xml
<!-- -->

    <StackPanel>
        <TextBox x:Name="txtBox" Margin="10" FontSize="30"/>
        <Button  Click="button1_Click"  x:Name="button1" Margin="10" FontSize="30" Content="button1"/>
        <Button  Command="local:MyCommand.cmdAction"    x:Name="button2" Margin="10" FontSize="30" Content="button2"/>
    </StackPanel>
</Window>
```

```csharp
// ICommand의 구현체여야 한다.

public class ActionCommand : ICommand
{
    public event EventHandler CanExecuteChanged;

    public bool CanExecute(object parameter)
    {
        // 항상 Enabled 상태이며
        return true;
        //return false;
    }

    public void Execute(object parameter)
    {
        // Action시 Messagebox를 띄운다.
        MessageBox.Show("ActionCommand");
    }
}

// ...
```

---

## Command를 사용하면 글자가 비워지면 Button이 Disabled 못하는 것인가?

```csharp
public class ActionCommand2 : ICommand
{
    public event EventHandler CanExecuteChanged;

    public TextBox txtBox = null;

    public void FireCanExecute()
    {
        // CanExecute 를 다시 호출해 달라는 의미.
        CanExecuteChanged(this, EventArgs.Empty);
    }

    public bool CanExecute(object parameter)
    {
        Console.WriteLine("CanExecute");

        if (txtBox == null)
            txtBox = ((Command3Window)Application.Current.MainWindow).txtBox;

        return !string.IsNullOrEmpty(txtBox.Text);
    }

    public void Execute(object parameter)
    {
        MessageBox.Show(txtBox?.Text);
    }
}

static class MyCommand2
{
    public static ActionCommand2 cmdAction = new ActionCommand2();
}



public partial class Command3Window : Window
{
    public Command3Window()
    {
        InitializeComponent();
    }

    private void txtBox_TextChanged(object sender, TextChangedEventArgs e)
    {
        MyCommand2.cmdAction.FireCanExecute();
    }
}
```

---

```csharp
public class ActionCommand3 : ICommand
{
    public event EventHandler CanExecuteChanged
    {
        add
        {
            // CommandManager에게 CanExecute가 변경되었음을 알려준다.
            CommandManager.RequerySuggested += value;
        }

        remove
        {
            // CommandManager에게 CanExecute가 변경되었음을 알려준다.
            CommandManager.RequerySuggested -= value;
        }
    }




    public TextBox txtBox = null;
        
            
    public bool CanExecute(object parameter)
    {
        Console.WriteLine("CanExecute");

        if (txtBox == null)
            txtBox = ((Command4Window)Application.Current.MainWindow).txtBox;

        return !string.IsNullOrEmpty(txtBox.Text);
    }


    public void Execute(object parameter)
    {
        MessageBox.Show(txtBox?.Text);
    }
}

static class MyCommand3
{
    public static ActionCommand3 cmdAction = new ActionCommand3();
}

public partial class Command4Window : Window
{
    public Command4Window()
    {
        InitializeComponent();
    }
}
```

---

## Routed Command

* [Get Code 🌎](https://github.com/Arthur880708/WPF-Example/tree/master/18.Command/18.2.RoutedCommand)

```xml
<!-- -->

    <StackPanel>
        <TextBox x:Name="txtBox" Margin="10" FontSize="30"/>
        <Button  Command="local:MyCommand.cmdAction" x:Name="button" Margin="10" FontSize="30" Content="button1"/>
    </StackPanel>
</Window>
```

```csharp
public static class MyCommand
{
    public static RoutedCommand cmdAction = new RoutedCommand();

}

public partial class RoutedCommand1 : Window
{
    public RoutedCommand1()
    {
        InitializeComponent();

        CommandBinding cmd = new CommandBinding(MyCommand.cmdAction);

        cmd.Executed += Cmd_Executed;
        cmd.CanExecute += Cmd_CanExecute;

        this.CommandBindings.Add(cmd);

        InputBinding ibFind = new InputBinding(MyCommand.cmdAction, 
                                            new KeyGesture(Key.R, ModifierKeys.Control));
        this.InputBindings.Add(ibFind);
    }

    private void Cmd_CanExecute(object sender, CanExecuteRoutedEventArgs e)
    {
        e.CanExecute = !string.IsNullOrEmpty(txtBox.Text);
    }

    private void Cmd_Executed(object sender, ExecutedRoutedEventArgs e)
    {
        MessageBox.Show(txtBox.Text);
    }
}
```

---

* xaml에서 Command를 바인딩 할 수 있다.

```xml
<!-- -->

    <Window.Resources>
        <RoutedCommand x:Key="cmdAction" />
    </Window.Resources>


    <Window.CommandBindings>
        <CommandBinding Command="{StaticResource ResourceKey=cmdAction}" Executed="Cmd_Executed" 
                                                                         CanExecute="Cmd_CanExecute"/>
    </Window.CommandBindings>

    <Window.InputBindings>
        <KeyBinding Key="R" Modifiers="Control" Command="{StaticResource ResourceKey=cmdAction}"/>
    </Window.InputBindings>

    <StackPanel>
        <TextBox x:Name="txtBox" Margin="10" FontSize="30"/>
        <Button  Command="{StaticResource ResourceKey=cmdAction}"  x:Name="button" Margin="10" FontSize="30" Content="button1"/>
    </StackPanel>
</Window>

```

```csharp
public partial class RoutedCommand2 : Window
{
    public RoutedCommand2()
    {
        InitializeComponent();
    }

    private void Cmd_CanExecute(object sender, CanExecuteRoutedEventArgs e)
    {
        e.CanExecute = !string.IsNullOrEmpty(txtBox.Text);
    }

    private void Cmd_Executed(object sender, ExecutedRoutedEventArgs e)
    {
        MessageBox.Show(txtBox.Text);
    }
}
```