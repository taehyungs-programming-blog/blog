---
title: "Qt : window(start) key 막기"
permalink: qt/core/disable-win-key/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-06-04 00:00:00 -0000
last_modified_at: 2020-06-04 00:00:00 -0000
header:
  teaser: /file/image/qt-page-teaser.gif
sidebar:
  title: "qt"
  nav: qt
tag:
  - qt
category:
  - Core
  - disable
  - winkey
excerpt: ""
---

* [참고사이트](https://stackoverflow.com/questions/45968724/how-to-disable-keys-in-windows-globally-using-qt)

```cpp
#include <QtWidgets>
#include <windows.h>
//link against user32.lib when compiling in MSVC
#ifdef _MSC_VER
#pragma comment(lib, "User32.lib")
#endif

class GlobalPrintScreenBlocker {
public:
    GlobalPrintScreenBlocker():mHKeyboardHook(NULL) {}
    //to avoid leaking the hook procedure handle
    ~GlobalPrintScreenBlocker(){ unblock(); }
    //hook callback function (called on every system-wide key press)
    static LRESULT CALLBACK LowLevelKeyboardProc(int nCode,
                                                 WPARAM wParam, LPARAM lParam) {
        if(nCode == HC_ACTION) {
            PKBDLLHOOKSTRUCT p = reinterpret_cast<PKBDLLHOOKSTRUCT>(lParam);
            if(p->vkCode == VK_SNAPSHOT) return 1; //block print-screen key
        }
        //this is not a message we are interested in
        return CallNextHookEx(NULL, //ignored paramater
                              nCode,
                              wParam,
                              lParam);
    }
    void block(){
        mHKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, //low-level keyboard hool
                                          &LowLevelKeyboardProc, //callback
                                          GetModuleHandle(NULL), 
                                          0);
    }
    void unblock(){
        if(mHKeyboardHook) UnhookWindowsHookEx(mHKeyboardHook);
        mHKeyboardHook = NULL;
    }

private:
    HHOOK mHKeyboardHook;
};

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);

    GlobalPrintScreenBlocker blocker;
    QPushButton button("Disable ScreenShot");
    button.setCheckable(true);
    QObject::connect(&button, &QPushButton::toggled, [&](bool isChecked){
        if(isChecked)
            blocker.block();
        else
            blocker.unblock();
    });
    button.show();

    return a.exec();
}
```