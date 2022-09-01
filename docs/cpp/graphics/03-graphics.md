---
layout: default
title: "3. Gussian Filter"
parent: "(Graphics🖼)"
grand_parent: C++
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Clone Code 🌎](https://github.com/EasyCoding-7/Dx11ExampleWithImgui/tree/master/03)

```s
# 이미지 처리용 라이브러리라 생각하자
$ vcpkg install stb:x64-windows
```

## 이미지 밝게 해보기

```cpp
/* *******************
* 읽어들인 이미지를 밝게 만들어 보기
******************* */

class Example
{
public:
	Example(HWND window, int width, int height)
	{
		// 이미지 읽어들이기
		image.ReadFromFile("image_1.jpg"); // 컴퓨터 속도가 느리다면 "image_1_360.jpg" 사용

		// 시간 측정
		const auto start_time = std::chrono::high_resolution_clock::now();

	    // 이미지를 밝게 해보자.
		for (int j = 0; j < image.height; j++)
		{
			for (int i = 0; i < image.width; i++)
			{
				const int idx = i + image.width * j;

				image.pixels[idx].v[0] = std::clamp(image.pixels[idx].v[0] * 1.5f, 0.0f, 1.0f);
				image.pixels[idx].v[1] = std::clamp(image.pixels[idx].v[1] * 1.5f, 0.0f, 1.0f);
				image.pixels[idx].v[2] = std::clamp(image.pixels[idx].v[2] * 1.5f, 0.0f, 1.0f);
			}
		}
```

---

## Guassian Filter 넣기

```cpp

```