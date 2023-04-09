---
layout: default
title: "12. VisualTreeHelper"
parent: "(WPF basic)"
grand_parent: "(WPF)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/WPF-Example/tree/master/16.Cotrol/VISUALTREE)

```csharp
// ...

private void Button_Click(object sender, RoutedEventArgs e)
{
    // 자식의 개수
    int cnt = VisualTreeHelper.GetChildrenCount(this);

    Console.WriteLine(cnt);

    var child = VisualTreeHelper.GetChild(this, 0);

    Console.WriteLine($"{child.GetType()}");
}

// ...
```

```csharp
// 재귀적으로 자식 출력

private void Button_Click(object sender, RoutedEventArgs e)
{
    ShowChild("", this);
}

private void ShowChild(string sep, DependencyObject obj)
{
    Console.WriteLine($"{sep}{obj.GetType()}");

    int cnt = VisualTreeHelper.GetChildrenCount(obj);

    for (int i = 0; i < cnt; i++)
    {
        var child = VisualTreeHelper.GetChild(obj, i);

        ShowChild(sep + " ", child);
    }
}
```

* 새로운 창을 열어서 자식을 출력

```csharp

```csharp
private void Button_Click(object sender, RoutedEventArgs e)
{
    VisualTree vt = new VisualTree(); 
    
    vt.Process(this);

    vt.Show();
}
```

```csharp
public partial class VisualTree : Window
{
    public VisualTree()
    {
        InitializeComponent();
    }
    public void Process(DependencyObject root)
    {            
        treeView.Items.Clear();
        ProcessElement(root, null);
    }

    private void ProcessElement(DependencyObject element, TreeViewItem previousItem)
    {            
        TreeViewItem item = new TreeViewItem();
        item.Header = element.GetType().Name;

        item.IsExpanded = true;

        if (previousItem == null)
            treeView.Items.Add(item);
        else
            previousItem.Items.Add(item);
                    
        for (int i = 0; i < VisualTreeHelper.GetChildrenCount(element); i++)
        {
            ProcessElement(VisualTreeHelper.GetChild(element, i), item);
        }
    }
}
```

