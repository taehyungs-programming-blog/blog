---
title: "(OpenSource - Telegram Qt UI) Style을 어떻게 입히는가?"
permalink: opens/telegram/style/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-03-14 00:00:00 -0000
last_modified_at: 2021-03-14 00:00:00 -0000
tag:
  - OpenSource
  - Telegram
  - Qt
  - UI
category:
  - style
sidebar:
  - title: ""
  - nav:
classes: wide
excerpt: ""
header:
  teaser: /file/image/OpenS-page-teaser.gif
---

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
	const style::LinkButton &_st;       // style을 통해서 ui 세팅값을 가져온다.
	QString _text;
	int _textWidth = 0;
	std::optional<QColor> _textFgOverride;

};
```

style은 generate_styles.cmake에서 .style파일을 .h/.cpp로 변환해준다.<br>
이 방법이 그리 효율적인지는 잘 ...