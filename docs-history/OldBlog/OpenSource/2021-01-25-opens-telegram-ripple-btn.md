---
title: "(OpenSource - Telegram Qt UI) Ripple Button"
permalink: opens/telegram/ripple-btn/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-01-25 00:00:00 -0000
last_modified_at: 2021-01-25 00:00:00 -0000
tag:
  - OpenSource
  - Telegram
  - Qt
  - UI
category:
  - Ripple Button
sidebar:
  - title: ""
  - nav:
classes: wide
excerpt: ""
header:
  teaser: /file/image/OpenS-page-teaser.gif
---

## 일반 버튼

다 설명할순 없고 `LinkButton`를 대표로 설명

```cpp
class LinkButton : public AbstractButton {
public:
	LinkButton(QWidget *parent, const QString &text, const style::LinkButton &st = st::defaultLinkButton);

	int naturalWidth() const override;

	void setText(const QString &text);
	void setColorOverride(std::optional<QColor> textFg);

protected:
	void paintEvent(QPaintEvent *e) override;

	void onStateChanged(State was, StateChangeSource source) override;

private:
	const style::LinkButton &_st;   // 해당 UI를 style을 통해서 관리한다.
	QString _text;
	int _textWidth = 0;
	std::optional<QColor> _textFgOverride;

};
```

```cpp
LinkButton::LinkButton(
	QWidget *parent,
	const QString &text,
	const style::LinkButton &st)
: AbstractButton(parent)
, _st(st)
, _text(text)
, _textWidth(st.font->width(_text)) {
	resize(
		naturalWidth(),
		_st.padding.top() + _st.font->height + _st.padding.bottom());
	setCursor(style::cur_pointer);
}

int LinkButton::naturalWidth() const {
	return _st.padding.left() + _textWidth + _st.padding.right();
}

void LinkButton::paintEvent(QPaintEvent *e) {
	Painter p(this);

	const auto &font = (isOver() ? _st.overFont : _st.font);
	const auto pen = _textFgOverride.has_value()
		? QPen(*_textFgOverride)
		: isOver()
		? _st.overColor
		: _st.color;
	p.setFont(font);
	p.setPen(pen);
	const auto left = _st.padding.left();
	const auto top = _st.padding.top() + font->ascent;
	if (width() < naturalWidth()) {
		const auto available = width() - left - _st.padding.right();
		p.drawText(left, top, font->elided(_text, available));
	} else {
		p.drawText(left, top, _text);
	}
}

void LinkButton::setText(const QString &text) {
	_text = text;
	_textWidth = _st.font->width(_text);
	resize(naturalWidth(), _st.font->height);
	update();
}

void LinkButton::setColorOverride(std::optional<QColor> textFg) {
	_textFgOverride = textFg;
	update();
}

void LinkButton::onStateChanged(State was, StateChangeSource source) {
	update();
}
```

---

## Ripple 버튼

![](/file/image/telegram-ripple-btn-1.gif)

```cpp
class RippleButton : public AbstractButton {
public:
	RippleButton(QWidget *parent, const style::RippleAnimation &st);

	void setForceRippled(
		bool rippled,
		anim::type animated = anim::type::normal);
	bool forceRippled() const {
		return _forceRippled;
	}

	static QPoint DisabledRippleStartPosition() {
		return QPoint(-0x3FFFFFFF, -0x3FFFFFFF);
	}

	void clearState() override;

	void paintRipple(QPainter &p, int x, int y, const QColor *colorOverride = nullptr);

	void finishAnimating();

	~RippleButton();

protected:
	void onStateChanged(State was, StateChangeSource source) override;

	virtual QImage prepareRippleMask() const;
	virtual QPoint prepareRippleStartPosition() const;

private:
	void ensureRipple();

	const style::RippleAnimation &_st;
	std::unique_ptr<RippleAnimation> _ripple;
	bool _forceRippled = false;
	rpl::lifetime _forceRippledSubscription;

};
```

`std::unique_ptr<RippleAnimation> _ripple;`을 통해서 ripple을 그리게 된다.

```cpp
void RippleButton::paintRipple(QPainter &p, int x, int y, const QColor *colorOverride) {
	if (_ripple) {
		_ripple->paint(p, x, y, width(), colorOverride);
		if (_ripple->empty()) {
			_ripple.reset();
		}
	}
}
```

```cpp
void RippleAnimation::paint(QPainter &p, int x, int y, int outerWidth, const QColor *colorOverride) {
	if (_ripples.empty()) {
		return;
	}

	if (style::RightToLeft()) {
		x = outerWidth - x - (_mask.width() / style::DevicePixelRatio());
	}
	p.translate(x, y);
	for (const auto &ripple : _ripples) {
		ripple->paint(p, _mask, colorOverride);
	}
	p.translate(-x, -y);
	clearFinished();
}
```

```cpp
class RippleAnimation::Ripple {
public:
	Ripple(const style::RippleAnimation &st, QPoint origin, int startRadius, const QPixmap &mask, Fn<void()> update);
	Ripple(const style::RippleAnimation &st, const QPixmap &mask, Fn<void()> update);

	void paint(QPainter &p, const QPixmap &mask, const QColor *colorOverride);

	void stop();
	void unstop();
	void finish();
	void clearCache();
	bool finished() const {
		return _hiding && !_hide.animating();
	}

private:
	const style::RippleAnimation &_st;
	Fn<void()> _update;

	QPoint _origin;
	int _radiusFrom = 0;
	int _radiusTo = 0;

	bool _hiding = false;
	Ui::Animations::Simple _show;
	Ui::Animations::Simple _hide;
	QPixmap _cache;
	QImage _frame;

};
```

```cpp
void RippleAnimation::Ripple::paint(QPainter &p, const QPixmap &mask, const QColor *colorOverride) {
	auto opacity = _hide.value(_hiding ? 0. : 1.);
	if (opacity == 0.) {
		return;
	}

	if (_cache.isNull() || colorOverride != nullptr) {
		auto radius = anim::interpolate(_radiusFrom, _radiusTo, _show.value(1.));
		_frame.fill(Qt::transparent);
		{
			QPainter p(&_frame);
			p.setPen(Qt::NoPen);
			if (colorOverride) {
				p.setBrush(*colorOverride);
			} else {
				p.setBrush(_st.color);
			}
			{
				PainterHighQualityEnabler hq(p);
				p.drawEllipse(_origin, radius, radius);
			}
			p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
			p.drawPixmap(0, 0, mask);
		}
		if (radius == _radiusTo && colorOverride == nullptr) {
			_cache = PixmapFromImage(std::move(_frame));
		}
	}
	auto saved = p.opacity();
	if (opacity != 1.) p.setOpacity(saved * opacity);
	if (_cache.isNull()) {
		p.drawImage(0, 0, _frame);
	} else {
		p.drawPixmap(0, 0, _cache);
	}
	if (opacity != 1.) p.setOpacity(saved);
}
```