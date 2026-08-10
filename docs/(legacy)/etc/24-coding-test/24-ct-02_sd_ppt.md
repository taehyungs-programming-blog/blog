---
layout: default
title: "[System Design] PPT"
parent: "24년 Coding Test 🦖"
grand_parent: "Etc 🛠"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Q

* 파워포인트와 같은 프레젠테이션 소프트웨어의 도형 편집 기능을 구현하세요. 
* 다음 기능들이 필요합니다:
    * 기본 도형 생성 (사각형, 원, 삼각형 등)
    * 도형 이동, 크기 조절, 회전
    * Undo/Redo 기능
    * 도형 레이어 관리 (앞으로 가져오기, 뒤로 보내기)
    * 도형 그룹화/그룹 해제

---

## A

* 설계 패턴:
    * Command 패턴: Undo/Redo 구현
    * Composite 패턴: 도형 그룹화
    * Prototype 패턴: 도형 복제
* 주요 기능:
    * 도형의 기본 조작 (이동, 크기 조절, 회전)
    * 레이어 관리 (Z-index)
    * 그룹화/그룹 해제
    * 실행 취소/다시 실행
* 확장성:
    * 새로운 도형 타입 추가 용이
    * 새로운 명령어 추가 용이
* 성능 고려사항:
    * 도형 검색을 위한 공간 분할 구조 추가 가능
    * 대량의 도형 처리를 위한 최적화 필요
* 이 구현은 실제 프레젠테이션 소프트웨어의 핵심 기능을 보여줍니다. 실제 구현에서는 다음과 같은 추가 기능이 필요할 수 있습니다:
    * 도형 스타일 (선 스타일, 채우기 색상 등)
    * 선택 영역 처리
    * 도형 정렬
    * 충돌 감지
    * 스냅 기능

```cpp
#pragma once
#include "shape.h"
#include <stack>
#include <memory>
#include <vector>

class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
};

class MoveCommand : public Command {
public:
    MoveCommand(std::shared_ptr<Shape> shape, double dx, double dy);
    void execute() override;
    void undo() override;
    
private:
    std::shared_ptr<Shape> shape;
    double dx, dy;
};

class ShapeGroup : public Shape {
public:
    void addShape(std::shared_ptr<Shape> shape);
    void removeShape(std::shared_ptr<Shape> shape);
    void move(double dx, double dy) override;
    void resize(double scale) override;
    void rotate(double angle) override;
    bool contains(Point p) const override;
    std::shared_ptr<Shape> clone() const override;
    
private:
    std::vector<std::shared_ptr<Shape>> shapes;
};

class Editor {
public:
    void addShape(std::shared_ptr<Shape> shape);
    void removeShape(std::shared_ptr<Shape> shape);
    void moveShape(std::shared_ptr<Shape> shape, double dx, double dy);
    void resizeShape(std::shared_ptr<Shape> shape, double scale);
    void rotateShape(std::shared_ptr<Shape> shape, double angle);
    void groupShapes(const std::vector<std::shared_ptr<Shape>>& shapes);
    void ungroupShapes(std::shared_ptr<ShapeGroup> group);
    void bringToFront(std::shared_ptr<Shape> shape);
    void sendToBack(std::shared_ptr<Shape> shape);
    
    void undo();
    void redo();
    
private:
    std::vector<std::shared_ptr<Shape>> shapes;
    std::stack<std::unique_ptr<Command>> undoStack;
    std::stack<std::unique_ptr<Command>> redoStack;
};
```

```cpp
#pragma once
#include <memory>
#include <vector>
#include <string>

struct Point {
    double x, y;
};

class Shape {
public:
    virtual ~Shape() = default;
    virtual void move(double dx, double dy) = 0;
    virtual void resize(double scale) = 0;
    virtual void rotate(double angle) = 0;
    virtual bool contains(Point p) const = 0;
    virtual std::shared_ptr<Shape> clone() const = 0;
    
    void setZIndex(int z) { zIndex = z; }
    int getZIndex() const { return zIndex; }
    
protected:
    Point position{0, 0};
    double rotation{0};
    double scale{1.0};
    int zIndex{0};
};

class Rectangle : public Shape {
public:
    Rectangle(double width, double height);
    void move(double dx, double dy) override;
    void resize(double scale) override;
    void rotate(double angle) override;
    bool contains(Point p) const override;
    std::shared_ptr<Shape> clone() const override;
    
private:
    double width, height;
};

class Circle : public Shape {
public:
    explicit Circle(double radius);
    void move(double dx, double dy) override;
    void resize(double scale) override;
    void rotate(double angle) override;
    bool contains(Point p) const override;
    std::shared_ptr<Shape> clone() const override;
    
private:
    double radius;
};
```

```cpp
#include "editor.h"

void Editor::addShape(std::shared_ptr<Shape> shape) {
    shapes.push_back(shape);
    shape->setZIndex(shapes.size() - 1);
}

void Editor::moveShape(std::shared_ptr<Shape> shape, double dx, double dy) {
    auto cmd = std::make_unique<MoveCommand>(shape, dx, dy);
    cmd->execute();
    undoStack.push(std::move(cmd));
    redoStack = std::stack<std::unique_ptr<Command>>();
}

void Editor::bringToFront(std::shared_ptr<Shape> shape) {
    int maxZ = -1;
    for (const auto& s : shapes) {
        maxZ = std::max(maxZ, s->getZIndex());
    }
    shape->setZIndex(maxZ + 1);
}

void Editor::sendToBack(std::shared_ptr<Shape> shape) {
    int minZ = INT_MAX;
    for (const auto& s : shapes) {
        minZ = std::min(minZ, s->getZIndex());
    }
    shape->setZIndex(minZ - 1);
}

void Editor::groupShapes(const std::vector<std::shared_ptr<Shape>>& selectedShapes) {
    auto group = std::make_shared<ShapeGroup>();
    for (const auto& shape : selectedShapes) {
        group->addShape(shape);
        auto it = std::find(shapes.begin(), shapes.end(), shape);
        if (it != shapes.end()) {
            shapes.erase(it);
        }
    }
    addShape(group);
}

void Editor::undo() {
    if (!undoStack.empty()) {
        auto cmd = std::move(undoStack.top());
        undoStack.pop();
        cmd->undo();
        redoStack.push(std::move(cmd));
    }
}

void Editor::redo() {
    if (!redoStack.empty()) {
        auto cmd = std::move(redoStack.top());
        redoStack.pop();
        cmd->execute();
        undoStack.push(std::move(cmd));
    }
}
```