---
title: "(OpenSource - Telegram Qt UI) Fade Widget"
permalink: opens/telegram/fade-widget/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-01-26 00:00:00 -0000
last_modified_at: 2021-01-26 00:00:00 -0000
tag:
  - OpenSource
  - Telegram
  - Qt
  - UI
category:
  - Fade Widget
sidebar:
  - title: ""
  - nav:
classes: wide
excerpt: ""
header:
  teaser: /file/image/OpenS-page-teaser.gif
---

```cpp
class FadeAnimation {
public:
	FadeAnimation(TWidget *widget, float64 scale = 1.);

	bool paint(QPainter &p);
	void refreshCache();

	using FinishedCallback = Fn<void()>;
	void setFinishedCallback(FinishedCallback &&callback);

	using UpdatedCallback = Fn<void(float64)>;
	void setUpdatedCallback(UpdatedCallback &&callback);

	void show();
	void hide();

	void fadeIn(int duration);
	void fadeOut(int duration);

	void finish() {
		stopAnimation();
	}

	bool animating() const {
		return _animation.animating();
	}
	bool visible() const {
		return _visible;
	}

private:
	void startAnimation(int duration);
	void stopAnimation();

	void updateCallback();
	QPixmap grabContent();

	TWidget *_widget = nullptr;
	float64 _scale = 1.;

	Ui::Animations::Simple _animation;
	QSize _size;
	QPixmap _cache;
	bool _visible = false;

	FinishedCallback _finishedCallback;
	UpdatedCallback _updatedCallback;

};
```

```cpp
bool FadeAnimation::paint(QPainter &p) {
	if (_cache.isNull()) return false;

	const auto cache = _cache;
	auto opacity = _animation.value(_visible ? 1. : 0.);
	p.setOpacity(opacity);
	if (_scale < 1.) {
		PainterHighQualityEnabler hq(p);
		auto targetRect = QRect(
			(1 - kWideScale) / 2 * _size.width(),
			(1 - kWideScale) / 2 * _size.height(),
			kWideScale * _size.width(),
			kWideScale * _size.height());
		auto scale = opacity + (1. - opacity) * _scale;
		auto shownWidth = anim::interpolate(
			(1 - kWideScale) / 2 * _size.width(),
			0,
			scale);
		auto shownHeight = anim::interpolate(
			(1 - kWideScale) / 2 * _size.height(),
			0,
			scale);
		auto margins = QMargins(
			shownWidth,
			shownHeight,
			shownWidth,
			shownHeight);
		p.drawPixmap(targetRect.marginsAdded(margins), cache);
	} else {
		p.drawPixmap(0, 0, cache);
	}
	return true;
}
```