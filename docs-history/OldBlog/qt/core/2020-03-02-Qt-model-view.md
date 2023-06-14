---
title: "Qt : Model and View"
permalink: qt/ui/model_view/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-05-02 00:00:00 -0000
last_modified_at: 2020-05-02 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

## View, Model에 관해서 ... (이론)

* View는 데이터를 전시하는 Widget을 의미하고
* Model은 View에 맞게 데이터를 관리해주는 도구를 의미한다.

---

## View

* 각 Widget들이 View라고 보면된다.
* QListWidget, QTableWidget ... 등이 있겠지?

예를들자면 ...

* [52. QListWidgetItem 사용](https://8bitscoding.github.io/Qt-GDI-S6-52/)

를 들 수 있겠다.

![](/file/image/qt_view_example0.png)

![](/file/image/qt_view_example.png)

QListWidget(View)을 생성하여 Widget에 데이터를 넣는다.

<center><img src="/file/image/qt-gdi-s6-52-image-1.png" width="50%" height="50%"></center>

```cpp
// item은 이렇게 넣을 수 있고
ui->listWidget->addItems(fruitList);

// item을 읽는 방법은
QString fruit = ui->listWidget->currentItem()->data(Qt::DisplayRole).toString();
```

또다른 예제..

* [53. QTableWidgetItem 사용](https://8bitscoding.github.io/Qt-GDI-S6-53/)
* [54. QTreeWidgetItem 사용](https://8bitscoding.github.io/Qt-GDI-S6-54/)


---

## Model(기본)

* Model은 View에 맞게 데이터를 관리해주는 도구를 의미한다.
* Qt에서 제공해 주는 Model로 예시를 보여주자면 ...

QFileSystemModel, QStringListModel, QSortFilterProxyModel 등이 있고 그 중 하나인 QFileSystemModel을 간단히 설명한다.

* [55. QFileSystemModel 사용](https://8bitscoding.github.io/Qt-GDI-S6-55/)

<center><img src="/file/image/qt-gdi-s6-55-image-1.png" width="50%" height="50%"></center>

```cpp
// widget.h
// ...

private:
    Ui::Widget *ui;
    QFileSystemModel * model;       // Qt에서 제공해주는 QFileSystemModel을 선언
};
```

```cpp
// widget.cpp

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    model = new QFileSystemModel(this);
    model->setReadOnly(false);

    model->setRootPath(QDir::currentPath());

    ui->treeView->setModel(model);      // 모델을 이렇게 선언.
    ui->treeView->setAlternatingRowColors(true);


    QModelIndex index = model->index(QDir::currentPath());

    ui->treeView->expand(index);
    ui->treeView->scrollTo(index);
    ui->treeView->resizeColumnToContents(0);
}
```


* [56. QStringListModel 사용](https://8bitscoding.github.io/Qt-GDI-S6-56/)
* [57. QSortFilterProxyModel 사용](https://8bitscoding.github.io/Qt-GDI-S6-57/)


---

## Model(심화)

* 그럼 모델을 개발자가 컨트롤해 보고싶다면?
* SubClassing을 제공해준다.

* [68. view drag and drop](https://8bitscoding.github.io/Qt-GDI-S6-68/)

<center><img src="/file/image/qt-gdi-s6-68-image-1.png" width="50%" height="50%"></center>

```cpp
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    // PersonalModel을 아래와 같이 선언.
    PersonModel * model = new PersonModel(this);

    // 각 View에 model을 지정해 준다.
    ui->listView->setModel(model);
    ui->listView->setDragEnabled(true);
    ui->listView->setAcceptDrops(true);

    ui->tableView->setModel(model);
    ui->tableView->setDragEnabled(true);
    ui->tableView->setAcceptDrops(true);

    ui->treeView->setModel(model);
    ui->treeView->setDragEnabled(true);
    ui->treeView->setAcceptDrops(true);
}
```

```cpp
// Model의 구현부
#ifndef PERSONMODEL_H
#define PERSONMODEL_H

#include <QAbstractListModel>

class PersonModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit PersonModel(QObject *parent = nullptr);

signals:

public slots:

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool insertRows(int row, int count, const QModelIndex &parent) override;
    bool removeRows(int row, int count, const QModelIndex &parent) override;


    //Drag and Drop

    // QAbstractItemModel interface
public:

    QStringList mimeTypes() const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                      int row, int column, const QModelIndex &parent) override;

    Qt::ItemFlags flags(const QModelIndex &index = QModelIndex()) const override;

private:
    QStringList persons;

};

#endif // PERSONMODEL_H
```

뭐 만들때 마다 달라질테니 이런식으로 만든다... 정도만 알고 있자.

* [69. 68에 personal model 적용](https://8bitscoding.github.io/Qt-GDI-S6-69/)
* [73. personal model treeview](https://8bitscoding.github.io/Qt-GDI-S6-73/)
* [77. 73 응용](https://8bitscoding.github.io/Qt-GDI-S6-77/)