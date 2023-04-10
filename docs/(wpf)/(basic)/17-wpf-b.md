---
layout: default
title: "17. Animation"
parent: "(WPF basic)"
grand_parent: "(WPF)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Code Animation

* [Get Code 🌎](https://github.com/Arthur880708/WPF-Example/tree/master/20.Animation)

```csharp
namespace ANIMATION
{
    public partial class CodeAnimation : Window
    {

        private Storyboard storyBoard = new Storyboard();

        public CodeAnimation()
        {
            InitializeComponent();

            DoubleAnimation anim1 = new DoubleAnimation();

            anim1.From = 50;
            anim1.To = 250;
            anim1.Duration = TimeSpan.FromSeconds(1);
            anim1.AutoReverse = true;

            Storyboard.SetTarget(anim1, redBall);
            Storyboard.SetTargetProperty(anim1, new PropertyPath("(Ellipse.Width)"));

            DoubleAnimation anim2 = new DoubleAnimation();

            anim2.From = 10;
            anim2.To = 200;
            anim2.Duration = TimeSpan.FromSeconds(1);
            anim2.AutoReverse = true;
            anim2.BeginTime = TimeSpan.FromSeconds(2);

            Storyboard.SetTarget(anim2, redBall);
            Storyboard.SetTargetProperty(anim2, new PropertyPath("(Canvas.Left)"));

            storyBoard.Children.Add(anim1);
            storyBoard.Children.Add(anim2);
        }


        
        private void button1_Click(object sender, RoutedEventArgs e)
        {
            DoubleAnimation anim = new DoubleAnimation();

            anim.From = 50;
            anim.To = 250;
            anim.Duration = TimeSpan.FromSeconds(1);
            anim.AutoReverse = true;
            anim.RepeatBehavior = RepeatBehavior.Forever;

            redBall.BeginAnimation(Ellipse.WidthProperty, anim);

        }
        private void button2_Click(object sender, RoutedEventArgs e)
        {
            DoubleAnimation anim = new DoubleAnimation();

            anim.From = 10;
            anim.To = 200;
            anim.Duration = TimeSpan.FromSeconds(1);
            anim.AutoReverse = true;
            anim.RepeatBehavior = RepeatBehavior.Forever;

            redBall.BeginAnimation(Canvas.LeftProperty, anim);
        }

        private void button3_Click(object sender, RoutedEventArgs e)
        {
            Storyboard sb = new Storyboard();

            DoubleAnimation anim1 = new DoubleAnimation();

            anim1.From = 50;
            anim1.To = 250;
            anim1.Duration = TimeSpan.FromSeconds(1);
            anim1.AutoReverse = true;
            anim1.RepeatBehavior = RepeatBehavior.Forever;

            Storyboard.SetTarget(anim1, redBall);
            Storyboard.SetTargetProperty(anim1, new PropertyPath("(Ellipse.Width)"));


            DoubleAnimation anim2 = new DoubleAnimation();

            anim2.From = 10;
            anim2.To = 200;
            anim2.Duration = TimeSpan.FromSeconds(1);
            anim2.AutoReverse = true;
            anim2.RepeatBehavior = RepeatBehavior.Forever;

            Storyboard.SetTarget(anim2, redBall);
            Storyboard.SetTargetProperty(anim2, new PropertyPath("(Canvas.Left)"));

            sb.Children.Add(anim1);
            sb.Children.Add(anim2);

            sb.Begin();

        }




        private void button4_Click(object sender, RoutedEventArgs e)
        {
            storyBoard.Begin();
        }

        private void button5_Click(object sender, RoutedEventArgs e)
        {
            storyBoard.Stop();
        }
    }

}
```

---

## xaml Animation

```xml
<!-- -->

    <Window.Resources>
        <BeginStoryboard x:Name="MyStoryBoard" x:Key="MyStory">
            <Storyboard>
                <DoubleAnimation Storyboard.TargetName ="redBall" 
                                             Storyboard.TargetProperty="(Canvas.Left)" From="10" To="200"  Duration="0:0:3"/>
            </Storyboard>
        </BeginStoryboard>

    </Window.Resources>
    
    
    
    <Canvas>
        <Ellipse Canvas.Left="10" Canvas.Top="10" x:Name="redBall" Width="50" Height="50" Fill="Red"/>
        
        <Button Content="button1" Canvas.Left="10" Canvas.Top="80"  Width="100" Height="30">
            <Button.Triggers>
                <EventTrigger RoutedEvent="Button.Click">

                    <BeginStoryboard x:Name="MyStoryBoard">
                        <Storyboard>
                            <DoubleAnimation Storyboard.TargetName ="redBall" 
                                             Storyboard.TargetProperty="(Canvas.Left)" From="10" To="200"  Duration="0:0:3"/>
                        </Storyboard>
                    </BeginStoryboard>

                </EventTrigger>
            </Button.Triggers>
        </Button>

        <Button Content="button2" Canvas.Left="10" Canvas.Top="120" Width="100" Height="30">
            <Button.Triggers>
                <EventTrigger RoutedEvent="Button.Click">

                    <StaticResource ResourceKey="MyStory"></StaticResource>
                    
                </EventTrigger>
            </Button.Triggers>
        </Button>
    </Canvas>
</Window>

```