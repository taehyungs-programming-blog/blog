---
layout: default
title: "07. Tree"
parent: "(자료구조 & 알고리즘)"
grand_parent: C#
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```csharp
class TreeNode<T>
{
    public T Data {get; set;}
    public List<TreeNode<T>> Children {get; set;} = new List<TreeNode<T>>();
}

static void Main(string[] args)
{
    // 트리만들기
    TreeNode<string> root = new TreeNode<string>() { Data = "R1 개발실" };
    {
        {
            TreeNode<string> node = new TreeNode<string>() { Data = "디자인팀" };
            node.Children.Add(new TreeNode<string>() { Data = "전투" });
            node.Children.Add(new TreeNode<string>() { Data = "경제" });
            node.Children.Add(new TreeNode<string>() { Data = "스토리" });
            root.Children.Add(node);
        }

        {
            TreeNode<string> node = new TreeNode<string>() { Data = "프로그래밍팀" };
            node.Children.Add(new TreeNode<string>() { Data = "서버" });
            node.Children.Add(new TreeNode<string>() { Data = "클라" });
            node.Children.Add(new TreeNode<string>() { Data = "엔진" });
            root.Children.Add(node);
        }

        {
            TreeNode<string> node = new TreeNode<string>() { Data = "아트팀" };
            node.Children.Add(new TreeNode<string>() { Data = "배경" });
            node.Children.Add(new TreeNode<string>() { Data = "케릭터" });
            root.Children.Add(node);
        }
    }

    PrintTree(root);
    GetHeight(root);
}

// 트리 순회하기
static void PrintTree(TreeNode<string> root)
{
    Console.WriteLine(root.Data);
    foreach(TreeNode<string> child in root.Children)
        PrintTree(child);
}

// 트리 높이구하기
static void GetHeight(TreeNode<string> root)
{
    int height = 0;

    foreach(TreeNode<string> child in root.Children)
    {
        int newHeight = GetHeight(child) + 1/*부모와 자신의 hegith 1추가*/;
        height = Math.Max(height, newHeight);
        /* 위와 동일표현
        if(height < newHeight>)
            height = newHeight;
        */
    }

    return height;
}
```
