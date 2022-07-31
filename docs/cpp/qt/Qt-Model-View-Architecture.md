---
layout: default
title: "Model View Architecture"
parent: (QT)
grand_parent: C++
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

🍓 **Model View Architecture**는 모두 정리하자면 내용이 너무 방대하다.<br>
🍓 필요한 내용위주로 간단간단하게 정리하고 **코드 위주**로 설명하겠다.(안그럼 내용이 너무많음 ㅠ)

<br>

🍓 결론부터 말하자면 Qt는 MVD(Model-View-Delegate)구조를 선호한다.<br>
🍓 **Model** : 데이터를 관리<br>
🍓 **View** : UI를 담을 틀을 의미<br>
🍓 **Delegate** : UI를 세부적으로 어떻게 그릴지 <br>
🍓 뭔 소리인가 싶겠지만 아래를 보면 알게 됨.

---

## List Widget Item 넣어보기

```cpp
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    ui->listWidget->setIconSize(QSize(70,70));

    fruitList << "Apple" << "Avocado" << "Banana"
              << "Blueberries" << "Cucumber" << "EggFruit"
              << "Fig" << "Grape" << "Mango" << "Pear"
              << "Pineapple" << "Watermellon";

    ui->listWidget->addItems(fruitList);

    for(int i =0 ; i < ui->listWidget->count() ; i++){
        QListWidgetItem * item = ui->listWidget->item(i);
        QString filename = ":/images/" + fruitList[i].toLower()+  ".png";
        item->setIcon(QIcon(filename));

        // item 별로 데이터를 넣을 수 있고
        item->setData(Qt::UserRole,fruitList[i]);

        // item에 label은 이렇게 선언가능
        item->setData(Qt::DisplayRole,fruitList[i]+"Funny123");
    }
}
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/qt/model-view-arc-1.png"/>
</p>

---

## Table Widget Item 넣어보기

```cpp
#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    // table은 2중 Vector 구조이다.
    table.append({"John","Doe","32","Farmer","Single","Gounduana","Mestkv","89"});
    table.append({"Mary","Jane","27","Teacher","Married","Verkso","Tukk","55"});
    table.append({"John","Doe","32","Farmer","Single","Gounduana","Mestkv","67"});
    // ...

    QStringList labels;

    labels << "First Name"<<"Last Name"<<"Age"<<"Proffession"<<"Marital Status"
               <<"Country"<<"City"<<"Social Score";

    ui->tableWidget->setHorizontalHeaderLabels(labels);

    int rows = table.size();
    int columns = table[0].size();

    for( int row = 0 ; row < rows ; row++){
        newRow();
        for( int col = 0 ; col < columns ; col++){
            //We are in a table cell
            ui->tableWidget->item(row, col)->setText(table[row][col]);
        }
    }

    ui->tableWidget->setAlternatingRowColors(true);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::newRow()
{
    int row = ui->tableWidget->rowCount();

    ui->tableWidget->insertRow(row);

    QTableWidgetItem * first = nullptr;

    for(int i =0; i < 8; i++){
        QTableWidgetItem * item = new QTableWidgetItem;
        if(i == 0)
            first = item;
        item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->tableWidget->setItem(row,i,item);
    }

    if(first)
        ui->tableWidget->setCurrentItem(first);

}
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/qt/model-view-arc-2.png"/>
</p>

---

## Tree Widget Item 넣어보기

```cpp
#include "widget.h"
#include "ui_widget.h"
#include <QDebug>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->treeWidget->setColumnCount(2);

    QTreeWidgetItem * googleRoot = addRootOrganization("Google Inc","Head Quarters");

    QTreeWidgetItem * googleIndia = addChildOrganization(googleRoot,"Google India","Google India Branch");

    addChildOrganization(googleIndia,"Mumbai","AI Research");

    addChildOrganization(googleIndia,"Bangalore","Sales");

    QTreeWidgetItem * googleGhana = addChildOrganization(googleRoot,"Google Ghana","Ghana Branch");
    addChildOrganization(googleGhana,"Akra","AI");
}

Widget::~Widget()
{
    delete ui;
}

QTreeWidgetItem *Widget::addRootOrganization(QString company, QString purpose)
{
    QTreeWidgetItem * item = new QTreeWidgetItem(ui->treeWidget);
    item->setText(0,company);
    item->setText(1,purpose);
    return item;

}

QTreeWidgetItem *Widget::addChildOrganization(QTreeWidgetItem *parent, QString branch, QString description)
{
    QTreeWidgetItem * item = new QTreeWidgetItem();
    item->setText(0, branch);
    item->setText(1, description);
    parent->addChild(item);
    return  item;

}

void Widget::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    qDebug() << " Clicked on :" << item->text(0) << " column : " << column;
}
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/qt/model-view-arc-3.png"/>
</p>

---

## File System View

```cpp
#include "widget.h"
#include "ui_widget.h"
#include <QInputDialog>
#include <QMessageBox>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    model = new QFileSystemModel(this);
    model->setReadOnly(false);

    /*
         * Sets the directory that is being watched by the model to newPath
         *  by installing a file system watcher on it. Any changes to files
         *  and directories within this directory will be reflected in the model.
         * */
    model->setRootPath(QDir::currentPath());

    ui->treeView->setModel(model);
    ui->treeView->setAlternatingRowColors(true);


    QModelIndex index = model->index(QDir::currentPath());

    ui->treeView->expand(index);
    ui->treeView->scrollTo(index);
    ui->treeView->resizeColumnToContents(0);


}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_addDirButton_clicked()
{

    QModelIndex index = ui->treeView->currentIndex();
    if (!index.isValid())
        return;

    QString dirName = QInputDialog::getText(this,
                                "Create Directory",
                                  "Directory name");

    if(!dirName.isEmpty()){

        if(!model->mkdir(index,dirName).isValid()){
            QMessageBox::information(this, "Create Directory",
                                "Failed to create the directory");
        }
    }
}

void Widget::on_removeFileDir_clicked()
{
    QModelIndex index = ui->treeView->currentIndex();
    if (!index.isValid())
        return;

    bool ok;
    //Check if it's a dir or file that you are deleting
    if (model->fileInfo(index).isDir()) {
        ok = model->rmdir(index);
    } else {
        ok = model->remove(index);
    }

    if (!ok)
           QMessageBox::information(this, "Delete",
                   QString("Failed to delete %1").arg(model->fileName(index)));
}

```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/qt/model-view-arc-4.png"/>
</p>

---

## StringList Model 써보기

```cpp
#include "widget.h"
#include "ui_widget.h"
#include <QDebug>


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    colorList = QColor::colorNames();

    model  = new QStringListModel(colorList,this);

    // listView는 QListView이고 Model을 StringListModel로 선언
    ui->listView->setModel(model);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_listView_clicked(const QModelIndex &index)
{
    QPixmap mPixmap(ui->label->size());
    mPixmap.fill(QColor(model->data(index,Qt::DisplayRole).toString()));
    ui->label->setPixmap(mPixmap);


    qDebug() << "Showing all the colors";
    qDebug() << "--------------------->>> Model Internal String list " << model->stringList();
    qDebug() << "--------------------->>> Original External String list " << colorList;

}
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/qt/model-view-arc-5.png"/>
</p>

---

## SortFilterProxyModel 써보기

```cpp
#include "widget.h"
#include "ui_widget.h"
#include <QDebug>


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    colorList = QColor::colorNames();

    /*
        private:
            Ui::Widget *ui;
            QStringListModel * model;
            QStringList colorList;
            QSortFilterProxyModel * proxyModel;
    */

    model  = new QStringListModel(colorList,this);

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);


    ui->listView->setModel(proxyModel);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_listView_clicked(const QModelIndex &index)
{
    QPixmap mPixmap(ui->label->size());
    mPixmap.fill(QColor(proxyModel->data(index,Qt::DisplayRole).toString()));
    ui->label->setPixmap(mPixmap);


    qDebug() << "Showing all the colors";
    qDebug() << "--------------------->>> Model Internal String list " << model->stringList();
    qDebug() << "--------------------->>> Original External String list " << colorList;

}

void Widget::on_matchStringLineEdit_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    proxyModel->setFilterRegExp(ui->matchStringLineEdit->text());
}

```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/qt/model-view-arc-6.png"/>
</p>

---

## (Custom Model) Read Only Model

🍓 하나의 Model을 여러 View로 표현하고자한다.<br>
🍓 + **Model을 통하여 데이터를 관리하고자 한다.**

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/qt/model-view-arc-7.png"/>
</p>

```cpp
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    PersonModel * model = new PersonModel(this);

    // View는 모두 다르며 Model만 같다
    ui->listView->setModel(model);
    ui->tableView->setModel(model);
    ui->treeView->setModel(model);
}
```

```cpp
#include "personmodel.h"

PersonModel::PersonModel(QObject *parent) : QAbstractListModel(parent)
{

    //Populate with initial data
    persons.append (new Person("Jamie Lannister","red",33));
    persons.append(new Person("Marry Lane","cyan",26));
    persons.append(new Person("Steve Moors","yellow",44));
    persons.append(new Person("Victor Trunk","dodgerblue",30));
    persons.append(new Person("Ariel Geeny","blue",33));
    persons.append(new Person("Knut Vikran","lightblue",26));

}

PersonModel::~PersonModel()
{
    qDeleteAll(persons);
}

int PersonModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return  persons.size();
}

QVariant PersonModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= persons.count())
        return QVariant();
    Person * person = persons[index.row()];
    if(role == Qt::DisplayRole){
        return  person->names() + " " + QString::number(person->age())
                +" " + person->favoriteColor();
    }

    if(role == Qt::ToolTipRole){
        return  person->names() + " " + QString::number(index.row());
    }
    return  QVariant();
}
```

---

## (Custom Model) Editable Model

```cpp
#include "personmodel.h"
#include <QDebug>

PersonModel::PersonModel(QObject *parent) : QAbstractListModel(parent)
{

    //Populate with initial data
    persons.append (new Person("Jamie Lannister","red",33));
    persons.append(new Person("Marry Lane","cyan",26));
    persons.append(new Person("Steve Moors","yellow",44));
    persons.append(new Person("Victor Trunk","dodgerblue",30));
    persons.append(new Person("Ariel Geeny","blue",33));
    persons.append(new Person("Knut Vikran","lightblue",26));

}

PersonModel::~PersonModel()
{
    qDeleteAll(persons);
}

int PersonModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return  persons.size();
}

QVariant PersonModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= persons.count())
        return QVariant();
    Person * person = persons[index.row()];
    if(role == Qt::DisplayRole){
        return  person->names();
    }

    if(role == Qt::EditRole){
        qDebug() << "Data method called with edit role";
         return  person->names();
    }

    if(role == Qt::ToolTipRole){
        return  person->names();
    }
    return  QVariant();

}

bool PersonModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid()) {
        return false;
    }

    Person * person = persons[index.row()];
    bool somethingChanged = false;

    switch (role) {

    case Qt::EditRole:
        {
            if( person->names()!= value.toString()){
                person->setNames(value.toString());
                somethingChanged = true;
            }
        }
    }

    if(somethingChanged){
        emit dataChanged(index,index);
        return true;
    }

    return false;
}

QVariant PersonModel::headerData(int section, Qt::Orientation orientation, int role) const
{

    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    if (orientation == Qt::Horizontal) {
        return QString("Person names");
    }
    // vertical rows
    return QString("Person %1").arg(section);
}

Qt::ItemFlags PersonModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QAbstractItemModel::flags(index);
    }
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/qt/model-view-arc-8.png"/>
</p>

---

## (Custom Model) Item 삭제/추가 가능하게 수정

```cpp
#include "widget.h"
#include "ui_widget.h"
#include <QInputDialog>
#include <QMessageBox>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    model = new PersonModel(this);

    ui->listView->setModel(model);
    ui->tableView->setModel(model);
    ui->treeView->setModel(model);

    ui->tableView->setSelectionModel(ui->listView->selectionModel());
    ui->treeView->setSelectionModel(ui->listView->selectionModel());

}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_addPersonButton_clicked()
{
    bool ok;
       QString name = QInputDialog::getText(nullptr, "Names",
                                            tr("Person name:"), QLineEdit::Normal,
                                            "Type in name", &ok);

       if (ok && !name.isEmpty()){

           int age = QInputDialog::getInt(nullptr,"Person Age","Age",20,15,120);

            Person * person = new Person(name,"blue",age,this);

            model->addPerson(person);
       }else{
           QMessageBox::information(nullptr,"Failure","Must specify name and age");
       }

}

void Widget::on_removePersonButton_clicked()
{
   QModelIndex index = ui->listView->currentIndex();
   model->removePerson(index);

}

```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/qt/model-view-arc-9.png"/>
</p>

---

## (Custom Model) Custom Role

```cpp
void Widget::on_addPersonButton_clicked()
{
    bool ok;
       QString name = QInputDialog::getText(nullptr, "Names",
                                            tr("Person name:"), QLineEdit::Normal,
                                            "Type in name", &ok);

       if (ok && !name.isEmpty()){

           int age = QInputDialog::getInt(nullptr,"Person Age","Age",20,15,120);

            Person * person = new Person(name,"blue",age,this);

            model->addPerson(person);
       }else{
           QMessageBox::information(nullptr,"Failure","Must specify name and age");
       }

}

void Widget::on_removePersonButton_clicked()
{
   QModelIndex index = ui->listView->currentIndex();
   model->removePerson(index);

}
```

```cpp
#include "personmodel.h"
#include <QDebug>

PersonModel::PersonModel(QObject *parent) : QAbstractListModel(parent)
{

    //Populate with initial data
    persons.append (new Person("Jamie Lannister","red",33));
    persons.append(new Person("Marry Lane","cyan",26));
    persons.append(new Person("Steve Moors","yellow",44));
    persons.append(new Person("Victor Trunk","dodgerblue",30));
    persons.append(new Person("Ariel Geeny","blue",33));
    persons.append(new Person("Knut Vikran","lightblue",26));

}

PersonModel::~PersonModel()
{
    qDeleteAll(persons);
}

void PersonModel::addPerson(Person *person)
{
    /*
    const int index = persons.size();
    beginInsertRows(QModelIndex(),index,index);
    persons.append(person);
    endInsertRows();
    */
    insertRows(persons.size(),1);
    setData(index(persons.size() -1),person->names(),NamesRole);
    setData(index(persons.size() -1),person->favoriteColor(),FavoriteColorRole);
    setData(index(persons.size() -1),person->age(),AgeRole);
}

void PersonModel::addPerson()
{
    Person *person = new Person("Added Person","yellowgreen",45,this);
    addPerson(person);
}

void PersonModel::addPerson(const QString &names, const int &age)
{
    Person *person=new Person(names,"yellowgreen",age);
    addPerson(person);
}

void PersonModel::removePerson(QModelIndex index)
{
    /*
    beginRemoveRows(QModelIndex(),index.row(),index.row());

    persons.removeAt(index.row());

    endRemoveRows();
    */
    removeRows(index.row(),1);

}

int PersonModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return  persons.size();
}

QVariant PersonModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= persons.count())
        return QVariant();
    Person * person = persons[index.row()];
    if(role == Qt::DisplayRole){
        return  person->names();
    }

    if(role == Qt::EditRole){
        qDebug() << "Data method called with edit role";
        return  person->names();
    }

    if(role == NamesRole){
        return  person->names();
    }

    if(role == FavoriteColorRole){
        return  person->favoriteColor();
    }
    if(role == AgeRole){
        return person->age();
    }

    if(role == Qt::ToolTipRole){
        return  person->names();
    }
    return  QVariant();

}

bool PersonModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid()) {
        return false;
    }

    Person * person = persons[index.row()];
    bool somethingChanged = false;

    switch (role) {

    case Qt::EditRole:
    {
        if( person->names()!= value.toString()){
            person->setNames(value.toString());
            somethingChanged = true;
        }
    }break;


    case NamesRole:
    {
        qDebug() << "Names role changing names, index " << index.row();
        if( person->names()!= value.toString()){
            person->setNames(value.toString());
            somethingChanged = true;
        }
    }
        break;


    case AgeRole:
    {
        if( person->age()!= value.toInt()){
            person->setAge(value.toInt());
            somethingChanged = true;
        }
    }
        break;

    case FavoriteColorRole:
    {
        if( person->favoriteColor()!= value.toString()){
            person->setFavoriteColor(value.toString());
            somethingChanged = true;
        }
    }
    }

    if(somethingChanged){
        emit dataChanged(index,index);
        return true;
    }

    return false;
}

QVariant PersonModel::headerData(int section, Qt::Orientation orientation, int role) const
{

    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    if (orientation == Qt::Horizontal) {
        return QString("Person names");
    }
    // vertical rows
    return QString("Person %1").arg(section);
}

Qt::ItemFlags PersonModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QAbstractItemModel::flags(index);
    }
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

bool PersonModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(QModelIndex(), row, row+count-1);

    for (int i = 0; i < count; ++i) {
        persons.insert(row,new Person());
    }
    endInsertRows();
    return  true;
}

bool PersonModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(QModelIndex(), row, row+count-1);

    for (int i = 0; i < count; ++i) {
        persons.removeAt(row);
    }
    endRemoveRows();
    return true;
}

QHash<int, QByteArray> PersonModel::roleNames() const
{
    QHash<int,QByteArray> roles;
    roles[NamesRole] = "names";
    roles[FavoriteColorRole] = "favoritecolor";
    roles[AgeRole] = "age";
    return  roles;
}

```

---

## (Custom Model) Delegate

🍓 **이제 Delegate를 통하여 세부적 UI를 어떻게 표현할지 정하려 한다.**

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/qt/model-view-arc-10.png"/>
</p>

```cpp
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    PersonDelegate * personDelegate = new PersonDelegate(this);

    model = new PersonModel(this);

    ui->listView->setModel(model);
    ui->tableView->setModel(model);
    
    // delegate를 붙인다.
    ui->tableView->setItemDelegateForColumn(2,personDelegate);
    ui->treeView->setModel(model);

    // delegate를 붙인다.
    ui->treeView->setItemDelegate(personDelegate);

    ui->tableView->setSelectionModel(ui->listView->selectionModel());
    ui->treeView->setSelectionModel(ui->listView->selectionModel());

}
```

```cpp
class PersonDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit PersonDelegate(QObject *parent = nullptr);


    // QAbstractItemDelegate interface
public:
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

```

```cpp
#include "persondelegate.h"
#include "personmodel.h"
#include <QComboBox>

PersonDelegate::PersonDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

QWidget *PersonDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                      const QModelIndex &index) const
{
    if(index.column() == 2){

        QComboBox *editor = new QComboBox(parent);

        foreach (QString color, QColor::colorNames()) {
            QPixmap mPix(50,50);
            mPix.fill(QColor(color));
            editor->addItem(QIcon(mPix),color);
        }

        return editor;
    }else{
        return QStyledItemDelegate::createEditor(parent,option,index);
    }

}

void PersonDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(index.column() == 2){
        QComboBox *combo = static_cast<QComboBox *>(editor);
        int idx = QColor::colorNames().indexOf(index.data(Qt::DisplayRole).toString());
        combo->setCurrentIndex(idx);
    }else{
        QStyledItemDelegate::setEditorData(editor,index);
    }

}

void PersonDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                  const QModelIndex &index) const
{
    if(index.column() == 2){
        QComboBox *combo = static_cast<QComboBox *>(editor);
        model->setData(index,combo->currentText(),PersonModel::FavoriteColorRole);
        //model->setData(index,combo->currentText(),Qt::EditRole);
    }else{
        QStyledItemDelegate::setModelData(editor,model,index);
    }

}

void PersonDelegate::updateEditorGeometry(QWidget *editor,
                                          const QStyleOptionViewItem &option,
                                          const QModelIndex &index) const
{
    Q_UNUSED(index);

    editor->setGeometry(option.rect);

}

QSize PersonDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QStyledItemDelegate::sizeHint(option, index).
            expandedTo(QSize(64,option.fontMetrics.height() + 10));
}

```

---

## Delegate Custom Painting

```cpp
#include "persondelegate.h"
#include "personmodel.h"
#include <QComboBox>
#include <QPainter>

PersonDelegate::PersonDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

QWidget *PersonDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                      const QModelIndex &index) const
{
    if(index.column() == 2){

        QComboBox *editor = new QComboBox(parent);

        foreach (QString color, QColor::colorNames()) {
            QPixmap mPix(50,50);
            mPix.fill(QColor(color));
            editor->addItem(QIcon(mPix),color);
        }

        return editor;
    }else{
        return QStyledItemDelegate::createEditor(parent,option,index);
    }

}

void PersonDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(index.column() == 2){
        QComboBox *combo = static_cast<QComboBox *>(editor);
        int idx = QColor::colorNames().indexOf(index.data(Qt::DisplayRole).toString());
        combo->setCurrentIndex(idx);
    }else{
        QStyledItemDelegate::setEditorData(editor,index);
    }

}

void PersonDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                  const QModelIndex &index) const
{
    if(index.column() == 2){
        QComboBox *combo = static_cast<QComboBox *>(editor);
        model->setData(index,combo->currentText(),PersonModel::FavoriteColorRole);
        //model->setData(index,combo->currentText(),Qt::EditRole);
    }else{
        QStyledItemDelegate::setModelData(editor,model,index);
    }

}

void PersonDelegate::updateEditorGeometry(QWidget *editor,
                                          const QStyleOptionViewItem &option,
                                          const QModelIndex &index) const
{
    Q_UNUSED(index);

    editor->setGeometry(option.rect);

}

QSize PersonDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QStyledItemDelegate::sizeHint(option, index).
            expandedTo(QSize(64,option.fontMetrics.height() + 10));
}

void PersonDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const
{
     if(index.column() == 2){

         //Highlight selected cell
         if (option.state & QStyle::State_Selected)
                     painter->fillRect(option.rect, option.palette.highlight());

         //Get fav color
         QString favColor = index.data(PersonModel::FavoriteColorRole).toString();

         painter->save();

         painter->setBrush(QBrush(QColor(favColor)));

         //Draw color rect
         painter->drawRect(option.rect.adjusted(3,3,-3,-3));

         //Text Size
         QSize textSize= option.fontMetrics.size(Qt::TextSingleLine,favColor);

         painter->setBrush(QBrush(QColor(Qt::white)));


         //Adustments for inner white rectangle
         int widthAdjust = (option.rect.width() - textSize.width())/2 - 3;
         int heightAdjust = (option.rect.height() - textSize.height())/2;

         painter->drawRect(option.rect.adjusted(widthAdjust,heightAdjust,
                                                -widthAdjust,-heightAdjust));

         painter->drawText(option.rect,favColor,Qt::AlignHCenter|Qt::AlignVCenter);


         painter->restore();

     }else{
         QStyledItemDelegate::paint(painter,option,index);
     }

}
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/qt/model-view-arc-11.png"/>
</p>