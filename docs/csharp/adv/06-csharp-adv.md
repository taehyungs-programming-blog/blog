---
layout: default
title: "6. Pattern Matching"
parent: (C# Adv)
grand_parent: "(C#)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## type pattern matching

```csharp
using System;

class Shape { }

class Circle : Shape
{
    public double radius = 100;
}
class Program
{
    public static void Draw(Shape s)
    {
        /*
        if ( s is Circle )
        {
            Circle c1 = (Circle)s;
            double d = c1.radius;
        }
        */
        // type pattern matching
        if (s is Circle c1)
        {
            double d = c1.radius;
        }

        // var pattern matching -> 이후에 swtich에서 사용됨.
        if (s is var c2) // var c2 = s
        {
            
        }

    }

    static void Main()
    {
        Draw(new Circle());       
    }
}
```

---

## const pattern matching

```csharp
using System;

class Circle { }

class Program
{
    public static void Main()
    {
        object o = new Circle();

        if ( o is Circle c1) { } // type pattern matching
        if ( o is var c2)    { } // var pattern matching


        int n = 10;
        
        if ( n is 10 ) // const pattern matching
        {
        }

        // 이거랑 뭐가 다른가??
            // IL 코드상으론 똑같음.
        if ( n == 10 )
        {
        }


        // 이런경우 사용된다.
        object obj = 10;

        //if ( obj == 10 ) // error
        //if (obj == (object)10) // ok

        if ((int)obj == 10) // ok
        {
            //Console.WriteLine("True");
        }
        //else
            //Console.WriteLine("False");

        if ( obj is 10)
        {
        }
    }
}
```

---

## Switch문과 Pattern Matching

```csharp
using System;

class Shape { }
class Circle : Shape { }

class Rectangle : Shape
{
    public double width = 100;
    public double height = 100;
}

class Program
{
    public static void Draw(Shape s)
    {
        switch (s)
        {
            // const pattern matching
            case null:
                break;

            // type pattern matching
            case Circle c:
                break;       


            case Rectangle r when r.width == r.height:
                break;
            
            case Rectangle r:
                break;

            default:
                break;
        }
    }








    public static void Main()
    {
        Draw(new Rectangle());

        //# 전통적인 switch 문의 구조
        int n = 1;
        switch (n)
        {
            case 1: 
                break;
            case 2: 
                break;
            default: 
                break;
        }
    }
}
```

```csharp
using System;
using System.Collections.Generic;

class Shape { }
class Circle : Shape { }

class Rectangle : Shape
{
    public double width = 100;
    public double height = 100;
}

class Program
{
    public static List<Shape> group = new List<Shape>();

    public static void Draw(Shape s)
    {
        switch (s)
        {
            case var r when (group.Contains(r)) :
                break;

            case Rectangle r:
                break;

            default: break;
        }
    }




    public static void Main()
    {
        Draw(new Rectangle());

    }
}
```

---

## switch expression

```csharp
using System;

class Program
{   
    public int square(int n)
    {
        return n * n;
    }
    public int square2(int n) => n * n;

    public static void Main()
    {
        int n = 50;

        //int s = n switch { 10 => 11, 20 => 22, 30 => 33, _ => 100  };

        int s = n switch { 
            10 => 11,   // n이 10일때 11을
            20 => 22, 
            30 => 33, 
            _ => 100 
        };
    }
}
```

```csharp
using System;

class Shape { }

class Rectangle : Shape 
{
    public double Width { set; get; } = 10;
    public double Height { set; get; } = 10;
}
class Circle : Shape
{
    public double Radius { set; get; } = 10;
}

class Point : Shape
{
    public double x = 0;
    public double y = 0;

    public void Deconstruct(out double ox, out double oy) => (ox, oy) = (x, y);
}

class Program
{
    public static void Main()
    {
        Shape s = new Circle();

        // type pattern matching
        double area = s switch 
        {
            null => 0,   // const pattern matching
            Point _ => 0,
            Circle c => Math.PI * c.Radius * c.Radius,
            Rectangle r => r.Width * r.Height,
            _ => 0
        };





        // tuple pattern
        int value1 = 0;
        int value2 = 0;

        var ret1 = (value1, value2) switch
        {
            (0, 0) => 0,
            var (a, b) when a > 100 => 100,
            var (a, b) when a <= 100 && b > 100 => 200,
            _ => 300
        };


        // positional pattern : Deconstructor 가 있는 타입
        Point pt = new Point();
        var (x1, y1) = pt;

        var ret2 = pt switch
        {
            (0, 0) => 0,
            var (a, b) when a > 100 => 100,
            var (a, b) when a <= 100 && b > 100 => 200,
            _ => 300
        };
    }
}
```