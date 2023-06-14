---
title: "(OpenSource - Telegram Qt UI) Slide Widget"
permalink: opens/telegram/slide-widget/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-01-27 00:00:00 -0000
last_modified_at: 2021-01-27 00:00:00 -0000
tag:
  - OpenSource
  - Telegram
  - Qt
  - UI
category:
  - Slide Widget
sidebar:
  - title: ""
  - nav:
classes: wide
excerpt: ""
header:
  teaser: /file/image/OpenS-page-teaser.gif
---

```cpp
QPixmap GrabWidget(not_null<QWidget*> target, QRect rect, QColor bg) {
	SendPendingMoveResizeEvents(target);
	if (rect.isNull()) {
		rect = target->rect();
	}

	auto result = QPixmap(rect.size() * style::DevicePixelRatio());
	result.setDevicePixelRatio(style::DevicePixelRatio());
	if (!target->testAttribute(Qt::WA_OpaquePaintEvent)) {
		result.fill(bg);
	}
	{
		QPainter p(&result);
		RenderWidget(p, target, QPoint(), rect);
	}
	return result;
}
```

```cpp
QPixmap SectionWidget::grabForShowAnimation(
		const SectionSlideParams &params) {
	return Ui::GrabWidget(this);
}
```

```cpp
void SectionWidget::showAnimated(
		SlideDirection direction,
		const SectionSlideParams &params) {
	if (_showAnimation) return;

	showChildren();
	auto myContentCache = grabForShowAnimation(params);
	hideChildren();
	showAnimatedHook(params);

	_showAnimation = std::make_unique<SlideAnimation>();
	_showAnimation->setDirection(direction);
	_showAnimation->setRepaintCallback([this] { update(); });
	_showAnimation->setFinishedCallback([this] { showFinished(); });
	_showAnimation->setPixmaps(
		params.oldContentCache,
		myContentCache);
	_showAnimation->setTopBarShadow(params.withTopBarShadow);
	_showAnimation->setWithFade(params.withFade);
	_showAnimation->start();

	show();
}
```

```cpp
void SectionWidget::paintEvent(QPaintEvent *e) {
	if (_showAnimation) {
		Painter p(this);
		_showAnimation->paintContents(p, e->rect());
	}
}
```