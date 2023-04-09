---
layout: default
title: "14. Data Binding"
parent: "(WPF basic)"
grand_parent: "(WPF)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/WPF-Example/tree/master/17.Binding_MVVM/DATABINDING1)

```csharp
public Ex1Window()
{
    InitializeComponent();

    person = new Person();
    person.Name = "kim";
    person.Address = "seoul";

    // stackpanel에 datacontext를 person으로 준다
    stackpanel.DataContext = person;
}

private void Button_Click(object sender, RoutedEventArgs e)
{
    person.Name = "lee";
    person.Address = "jeju";
}
```

```xml
<!-- -->

    <StackPanel x:Name="stackpanel">
        <TextBox Text="{Binding Path=Name}" FontSize="30"/>
        <TextBox Text="{Binding Path=Address}" FontSize="30"/>
        <Button Content="ClickMe" Click="Button_Click"/>
    </StackPanel>
    
</Window>
```

```csharp
public class Person : INotifyPropertyChanged
{
    private string name;
    private string address;

    public string Name
    {
        get { return name; }
        set { name = value;
            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs("Name"));
        }
    }

    public string Address
    {
        get { return address; }
        set { address = value;

            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs("Address"));

        }
    }

    // Property가 변경되면 등롤된 함수들에게 통보해 준다.
    public event PropertyChangedEventHandler PropertyChanged;
}
```

---

```xml
<!-- -->

    <StackPanel>
        <ListBox x:Name="listbox" DisplayMemberPath="Name" FontSize="20" />
        <Button Content="button1" Click="Button1_Click"/>
        <Button Content="button2" Click="Button2_Click"/>
    </StackPanel>
</Window>

```

```csharp
public partial class Ex2Window : Window
{
    //public List<Person> st = new List<Person>();

    public ObservableCollection<Person> st = new ObservableCollection<Person>();

    public Ex2Window()
    {
        InitializeComponent();

        st.Add(new Person { Name = "kim1", Address = "jeju1" });
        st.Add(new Person { Name = "kim2", Address = "jeju2" });

        listbox.ItemsSource = st;
    }

    private void Button1_Click(object sender, RoutedEventArgs e)
    {
        st[0].Name = "lee";
    }
    private void Button2_Click(object sender, RoutedEventArgs e)
    {
        st.Add(new Person { Name = "kim3", Address = "jeju3" });
    }
}
```

---

```xml
<!-- -->

    <Grid>
        <ListView Name="listview" FontSize="30">
            <ListView.View>
                <GridView>
                    <GridViewColumn Header="Name" Width="100"
                               DisplayMemberBinding="{Binding Path=Name}"/>
                    
                    <GridViewColumn Header="Address" Width="150"
                               DisplayMemberBinding="{Binding Path=Address}"/>
                </GridView>
            </ListView.View>
        </ListView>
    </Grid>
</Window>

```

```csharp
public partial class Ex3Window : Window
{
    public ObservableCollection<Person> st = new ObservableCollection<Person>();

    public Ex3Window()
    {
        InitializeComponent();

        st.Add(new Person { Name = "kim1", Address = "jeju1" });
        st.Add(new Person { Name = "kim2", Address = "jeju2" });
        st.Add(new Person { Name = "kim3", Address = "jeju3" });
        st.Add(new Person { Name = "kim4", Address = "jeju4" });

        listview.ItemsSource = st;
    }
}
```

---

```xml
<!-- -->
    
    <DataGrid x:Name="dataGrid" FontSize="30">
        
    </DataGrid>
    
</Window>

```

```csharp
public partial class Ex4Window : Window
{
    public ObservableCollection<Person> st = new ObservableCollection<Person>();

    public Ex4Window()
    {
        InitializeComponent();

        st.Add(new Person { Name = "kim1", Address = "jeju1" });
        st.Add(new Person { Name = "kim2", Address = "jeju2" });
        st.Add(new Person { Name = "kim3", Address = "jeju3" });
        st.Add(new Person { Name = "kim4", Address = "jeju4" });

        dataGrid.ItemsSource = st;
    }
}
```