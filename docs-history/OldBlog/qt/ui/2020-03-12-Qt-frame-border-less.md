---
title: "Qt - UI : Frameless(Borderless) Widget 만들기"
permalink: qt/frameless/ # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-12 00:00:00 -0000
last_modified_at: 2020-06-01 00:00:00 -0000
---

> * [Github](https://github.com/8bitscoding/VS_Frameless_Widget)

```cpp
// Frameless.h
#pragma once

#include <qdialog.h>
#include <QMainWindow>

#define DEFAULT_BORDER_SIZE 5

class QFramelessDialog : public QMainWindow
{
public:
	explicit QFramelessDialog(QWidget *parent = nullptr, 
								bool _isFrameless = true, 
								uint8_t _borderwidth = DEFAULT_BORDER_SIZE,
								bool _useResize = true,
								float _aspectratio = 0.0f);
	~QFramelessDialog() {}

private:
	bool nativeEvent(const QByteArray &eventType, void *message, long *result);
	bool event(QEvent *event);

	virtual const int getTitlebarHeight() = 0;
	virtual const int getTitlebarWidth() = 0;

	const uint8_t mBorderWidth;
	const bool mbResize;
	const float mbAspectRatio;
};
```

```cpp
// Frameless.cpp
#include "QFramelessDialog.h"

#include <Windows.h>					// for window MSG
#include <qmouseeventtransition.h>		// for qt-mouse-event
#include <dwmapi.h>						// for dwm-api

#pragma comment (lib,"Dwmapi.lib")

QFramelessDialog::QFramelessDialog(QWidget *parent, bool _isFrameless, uint8_t _borderwidth, bool _useResize, float _aspectratio)
	: QMainWindow(parent), mBorderWidth(_borderwidth), mbResize(_useResize), mbAspectRatio(_aspectratio)
{
	if (_isFrameless) {
		setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowMinMaxButtonsHint);
		setAttribute(Qt::WA_OpaquePaintEvent, false);
		setAttribute(Qt::WA_PaintOnScreen, true);
		setAttribute(Qt::WA_DontCreateNativeAncestors, true);
		setAttribute(Qt::WA_NativeWindow, true);
		setAttribute(Qt::WA_NoSystemBackground, false);		// UI 작업할때는 풀 것.
		setAttribute(Qt::WA_MSWindowsUseDirect3D, true);
		setAutoFillBackground(false);

		// intert shadow
		const MARGINS shadow = { 1, 1, 1, 1 };
		DwmExtendFrameIntoClientArea(HWND(winId()), &shadow);
	}
	else {
	}
}

bool QFramelessDialog::nativeEvent(const QByteArray &eventType, void *message, long *result) 
{
#if (QT_VERSION == QT_VERSION_CHECK(5, 11, 1))
	MSG* msg = *reinterpret_cast<MSG**>(message);
#else
	MSG* msg = reinterpret_cast<MSG*>(message);
#endif

	switch (msg->message)
	{
	case WM_NCCALCSIZE:
	{
		NCCALCSIZE_PARAMS& params = *reinterpret_cast<NCCALCSIZE_PARAMS*>(msg->lParam);
		if (params.rgrc[0].top != 0)
			params.rgrc[0].top -= 1;

		//this kills the window frame and title bar we added with WS_THICKFRAME and WS_CAPTION
		//*result = WVR_REDRAW;
		return true;
	}
	case WM_SIZING:
	{
		if (mbAspectRatio == 0) break;
		RECT* rect = (RECT*)msg->lParam;
		LONG width = rect->right - rect->left;
		LONG height = rect->bottom - rect->top;

		/*
		// if you want to setting min or max width, height
		// try to set below
		width = std::max(width, minWidth);
		width = std::min(width, maxWidth);
		height = std::max(height, minHeight);
		height = std::min(height, maxHeight);
		*/

		switch (msg->wParam)
		{
		case WMSZ_LEFT:
		case WMSZ_BOTTOMLEFT:
			rect->left = rect->right - width;
			rect->bottom = rect->top + width / mbAspectRatio;
			break;
		case WMSZ_RIGHT:
		case WMSZ_BOTTOMRIGHT:
			rect->right = rect->left + width;
			rect->bottom = rect->top + width / mbAspectRatio;
			break;
		case WMSZ_TOP:
		case WMSZ_TOPRIGHT:
			rect->top = rect->bottom - height;
			rect->right = rect->left + height * mbAspectRatio;
			break;
		case WMSZ_BOTTOM:
			rect->bottom = rect->top + height;
			rect->right = rect->left + height * mbAspectRatio;
			break;
		case WMSZ_TOPLEFT:
			rect->left = rect->right - width;
			rect->top = rect->bottom - width / mbAspectRatio;
			break;
		}
	}
	default:
		break;
	}
	return QMainWindow::nativeEvent(eventType, message, result);
}

bool QFramelessDialog::event(QEvent *event)
{
	QPoint widgetCursorPos = QWidget::mapFromGlobal(QCursor::pos());

	int x = widgetCursorPos.x();
	int y = widgetCursorPos.y();
	QSize size = this->size();

	switch (event->type())
	{
	case QEvent::HoverEnter:
	case QEvent::HoverMove:
		if (!mbResize) break;
		if (x < mBorderWidth && y > size.height() - mBorderWidth) {
			this->setCursor(QCursor(Qt::SizeBDiagCursor));
		}
		else if (x > size.width() - mBorderWidth && y > size.height() - mBorderWidth) {
			this->setCursor(QCursor(Qt::SizeFDiagCursor));
		}
		else if (x < mBorderWidth && y < mBorderWidth) {
			this->setCursor(QCursor(Qt::SizeFDiagCursor));
		}
		else if (x > size.width() - mBorderWidth && y < mBorderWidth) {
			this->setCursor(QCursor(Qt::SizeBDiagCursor));
		}
		else if (x < mBorderWidth) {
			this->setCursor(QCursor(Qt::SizeHorCursor));
		}
		else if (y < mBorderWidth) {
			this->setCursor(QCursor(Qt::SizeVerCursor));
		}
		else if (y > size.height() - mBorderWidth) {
			this->setCursor(QCursor(Qt::SizeVerCursor));
		}
		else if (x > size.width() - mBorderWidth) {
			this->setCursor(QCursor(Qt::SizeHorCursor));
		}
		else {
			this->setCursor(QCursor(Qt::ArrowCursor));
		}
		return true;
		break;
	case QEvent::HoverLeave:
		this->setCursor(QCursor(Qt::ArrowCursor));
		return true;
		break;
	case QEvent::MouseButtonPress:
		if (!mbResize) break;
		ReleaseCapture();
		if (x < mBorderWidth && y > size.height() - mBorderWidth) {
			this->setCursor(QCursor(Qt::SizeBDiagCursor));
			SendMessage(HWND(winId()), WM_NCLBUTTONDOWN, HTBOTTOMLEFT, 0);
		}
		else if (x > size.width() - mBorderWidth && y > size.height() - mBorderWidth) {
			this->setCursor(QCursor(Qt::SizeFDiagCursor));
			SendMessage(HWND(winId()), WM_NCLBUTTONDOWN, HTBOTTOMRIGHT, 0);
		}
		else if (x < mBorderWidth && y < mBorderWidth) {
			this->setCursor(QCursor(Qt::SizeFDiagCursor));
			SendMessage(HWND(winId()), WM_NCLBUTTONDOWN, HTTOPLEFT, 0);
		}
		else if (x > size.width() - mBorderWidth && y < mBorderWidth) {
			this->setCursor(QCursor(Qt::SizeBDiagCursor));
			SendMessage(HWND(winId()), WM_NCLBUTTONDOWN, HTTOPRIGHT, 0);
		}
		else if (y < mBorderWidth) {
			this->setCursor(QCursor(Qt::SizeVerCursor));
			SendMessage(HWND(winId()), WM_NCLBUTTONDOWN, HTTOP, 0);
		}
		else if (x < mBorderWidth) {
			this->setCursor(QCursor(Qt::SizeHorCursor));
			SendMessage(HWND(winId()), WM_NCLBUTTONDOWN, HTLEFT, 0);
		}
		else if (y > size.height() - mBorderWidth) {
			this->setCursor(QCursor(Qt::SizeVerCursor));
			SendMessage(HWND(winId()), WM_NCLBUTTONDOWN, HTBOTTOM, 0);
		}
		else if (x > size.width() - mBorderWidth) {
			this->setCursor(QCursor(Qt::SizeHorCursor));
			SendMessage(HWND(winId()), WM_NCLBUTTONDOWN, HTRIGHT, 0);
		}
		else if (y < getTitlebarHeight()) {
			SendMessage(HWND(winId()), WM_NCLBUTTONDOWN, HTCAPTION, 0);
		}
		return true;
		break;
	default:
		break;
	}
	return QWidget::event(event);
}
```

---

* [가장확실한 Example](https://github.com/Jorgen-VikingGod/Qt-Frameless-Window-DarkStyle)

> * [참고사이트1](https://github.com/deimos1877/BorderlessWindow)
> * [참고사이트2](https://github.com/JJPPeters/clTEM/blob/9d2f52f0ce556b1e1f4b70ac40cbfebc438d6f23/src/gui/controls/borderlessdialog.cpp)
> * [참고사이트3](https://github.com/Jorgen-VikingGod/Qt-Frameless-Window-DarkStyle)

사실 참고사이트에서 내가 많이 수정했기때문에 정말 참고용으로만 볼 것