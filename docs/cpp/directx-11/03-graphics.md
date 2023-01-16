---
layout: default
title: "03. Gussian Filter"
parent: "(DirectX 11 🎡)"
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

---

## 이미지를 읽어오는 코드

```cpp
void Image::ReadFromFile(const char* filename)
{
	// stb를 이용해 이미지 read
	unsigned char* img = stbi_load(filename, &width, &height, &channels, 0);

	if (width) {
		std::cout << width << " " << height << " " << channels << std::endl;
	}
	else {
		std::cout << "Error: reading " << filename << " failed." << std::endl;
	}

	// channels가 3(RGB) 또는 4(RGBA)인 경우만 가정
	// unsigned char(0에서 255)을 4채널 float(0.0f에서 1.0f)로 변환
	pixels.resize(width * height);
	for (int i = 0; i < width * height; i ++)
	{
		pixels[i].v[0] = img[i * channels] / 255.0f;
		pixels[i].v[1] = img[i * channels +1] / 255.0f;
		pixels[i].v[2] = img[i * channels +2] / 255.0f;
		pixels[i].v[3] = 1.0f;
	}

	delete [] img;
}
```

```cpp
void Update()
{
	// 이미지의 내용을 GPU 메모리로 복사
	D3D11_MAPPED_SUBRESOURCE ms;
	deviceContext->Map(canvasTexture, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	//memcpy(ms.pData, pixels.data(), pixels.size() * sizeof(Vec4));
	memcpy(ms.pData, image.pixels.data(), image.pixels.size() * sizeof(Vec4));
	deviceContext->Unmap(canvasTexture, NULL);
}
```

* 조금 더 설명하자면 ...

```cpp
device->CreateTexture2D(&textureDesc, nullptr, &canvasTexture);
// ID3D11Texture2D* canvasTexture = nullptr;
	// Device의 CreateTexture2D를 통해 Texture를 생성
	// Texture는 DirectX에서 Image를 그리기 위해 사용되는 Buffer라 이해하자

if (canvasTexture)
{
	device->CreateShaderResourceView(canvasTexture, nullptr, &canvasTextureView);
	// ID3D11ShaderResourceView* canvasTextureView = nullptr;
		// Texture에 접근하기 위한 Handle 생성

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	device->CreateRenderTargetView(canvasTexture, &renderTargetViewDesc, &canvasRenderTargetView);
	// ID3D11RenderTargetView* canvasRenderTargetView = nullptr;
		// Texture에 그려지는 RenderTargetView 생성
}
else
{
	std::cout << "CreateRenderTargetView() error" << std::endl;
}
```

---

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

## Convolution Kernel

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/graphics/graphics-3-1.png"/>
</p>

* 대략 이미지에 Convolution Kernel을 곱해서 새로운 이미지를 뽑아낸다고 생각하면된다.
* 그럼 Convolution Kernel만 알면 Filtter를 적용할 수 있겠네? -> Okay!

* Q? - 경계에 있는 부분은 어떻게 처리할까?

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/graphics/graphics-3-2.gif"/>
</p>

* A - 가까이있는 pixel을 가져온다.

---

## Box Blur

* [Clone Code 🌎](https://github.com/EasyCoding-7/Dx11ExampleWithImgui/tree/3/03)

* Convolution Kernel

```
1 1 1
1 1 1  x 1/9
1 1 1
```

```cpp
void Image::BoxBlur5()
{
	std::vector<Vec4> pixelsBuffer(this->pixels.size()); // 사본 복사

	/*
	* Separable convolution
	* 한 번에 2차원 Kernel을 적용하는 대신에 1차원 Kernel을 두 번 적용
	* 이해하기 쉽고 효율적이다.
	*/

	// 가로 방향 (x 방향)
#pragma omp parallel for
	for (int j = 0; j < this->height; j++)
	{
		for (int i = 0; i < this->width; i++)
		{
			// 주변 픽셀들의 색을 평균내어서 (i, j)에 있는 픽셀의 색을 변경
			// this->pixels로부터 읽어온 값들을 평균내어서 pixelsBuffer의 값들을 바꾸기
			Vec4 neightborColorSum{ 0.0f, 0.0f, 0.0f, 1.0f };
			for (int si = 0; si <= 5; si++)
			{
				Vec4 neightborColor = this->GetPixel(i + si - 2, j);
				neightborColorSum.v[0] += neightborColor.v[0];
				neightborColorSum.v[1] += neightborColor.v[1];
				neightborColorSum.v[2] += neightborColor.v[2];
			}

			pixelsBuffer[i + this->width * j].v[0] = neightborColorSum.v[0] * 0.2f /* 1/5 = 0.2 (5pixel의 평균) */;
			pixelsBuffer[i + this->width * j].v[1] = neightborColorSum.v[1] * 0.2f;
			pixelsBuffer[i + this->width * j].v[2] = neightborColorSum.v[2] * 0.2f;
		}
	}

	// Swap
	std::swap(this->pixels, pixelsBuffer);

	//return; // 여기까지 구현하고 테스트

	// 세로 방향 (y 방향)
#pragma omp parallel for
	for (int j = 0; j < this->height; j++)
	{
		for (int i = 0; i < this->width; i++)
		{
			// 주변 픽셀들의 색을 평균내어서 (i, j)에 있는 픽셀의 색을 변경
			// this->pixels로부터 읽어온 값들을 평균내어서 pixelsBuffer의 값들을 바꾸기

			Vec4 neightborColorSum{ 0.0f, 0.0f, 0.0f, 1.0f };
			for (int si = 0; si <= 5; si++)
			{
				Vec4 neightborColor = this->GetPixel(i, j + si - 2);
				neightborColorSum.v[0] += neightborColor.v[0];
				neightborColorSum.v[1] += neightborColor.v[1];
				neightborColorSum.v[2] += neightborColor.v[2];
			}

			pixelsBuffer[i + this->width * j].v[0] = neightborColorSum.v[0] * 0.2f /* 1/5 = 0.2 (5pixel의 평균) */;
			pixelsBuffer[i + this->width * j].v[1] = neightborColorSum.v[1] * 0.2f;
			pixelsBuffer[i + this->width * j].v[2] = neightborColorSum.v[2] * 0.2f;
		}
	}

	// Swap
	std::swap(this->pixels, pixelsBuffer);
}
```

---

## Guassian Blur

* [Clone Code 🌎](https://github.com/EasyCoding-7/Dx11ExampleWithImgui/tree/4/03)

* Convolution Kernel

```
1 2 1
2 4 2  x 1/16
1 2 1
```

```cpp
void Image::GaussianBlur5()
{
	std::vector<Vec4> pixelsBuffer(this->pixels.size());

	/*
	* 참고자료
	* https://en.wikipedia.org/wiki/Gaussian_filter
	* https://followtutorials.com/2013/03/gaussian-blurring-using-separable-kernel-in-c.html
	*/
	const float weights[5] = { 0.0545f, 0.2442f, 0.4026f, 0.2442f, 0.0545f };

	// 가로 방향 (x 방향)
#pragma omp parallel for
	for (int j = 0; j < this->height; j++)
	{
		for (int i = 0; i < this->width; i++)
		{
			// 주변 픽셀들의 색을 평균내어서 (i, j)에 있는 픽셀의 색을 변경
			// this->pixels로부터 읽어온 값들을 평균내어서 pixelsBuffer의 값들을 바꾸기

			Vec4 neightborColorSum{ 0.0f, 0.0f, 0.0f, 1.0f };
			for (int si = 0; si < 5; si++)
			{
				Vec4 neightborColor = this->GetPixel(i + si - 2, j);
				neightborColorSum.v[0] += neightborColor.v[0] * weights[si];
				neightborColorSum.v[1] += neightborColor.v[1] * weights[si];
				neightborColorSum.v[2] += neightborColor.v[2] * weights[si];
			}

			pixelsBuffer[i + this->width * j].v[0] = neightborColorSum.v[0];
			pixelsBuffer[i + this->width * j].v[1] = neightborColorSum.v[1];
			pixelsBuffer[i + this->width * j].v[2] = neightborColorSum.v[2];
		}
	}

	// Swap
	std::swap(this->pixels, pixelsBuffer);

	// 세로 방향 (y 방향)
#pragma omp parallel for
	for (int j = 0; j < this->height; j++)
	{
		for (int i = 0; i < this->width; i++)
		{
			// 주변 픽셀들의 색을 평균내어서 (i, j)에 있는 픽셀의 색을 변경
			// this->pixels로부터 읽어온 값들을 평균내어서 pixelsBuffer의 값들을 바꾸기
			Vec4 neightborColorSum{ 0.0f, 0.0f, 0.0f, 1.0f };
			for (int si = 0; si < 5; si++)
			{
				Vec4 neightborColor = this->GetPixel(i, j + si - 2);
				neightborColorSum.v[0] += neightborColor.v[0] * weights[si];
				neightborColorSum.v[1] += neightborColor.v[1] * weights[si];
				neightborColorSum.v[2] += neightborColor.v[2] * weights[si];
			}

			pixelsBuffer[i + this->width * j].v[0] = neightborColorSum.v[0];
			pixelsBuffer[i + this->width * j].v[1] = neightborColorSum.v[1];
			pixelsBuffer[i + this->width * j].v[2] = neightborColorSum.v[2];

		}
	}

	// Swap
	std::swap(this->pixels, pixelsBuffer);
}
```

---

## Bloom Effect

* [Clone Code 🌎](https://github.com/EasyCoding-7/Dx11ExampleWithImgui/tree/5/03)

* Gaussian Blur를 이용한다

1. 밝은 Pixel은 그대로 두고 어두운 Pixel을 완전히 검은색으로 변경한다.
2. 밝은 Pixel만 남은 이미지에 Gaussian Blur를 적용한다.
3. 원본이미지에 2번 이미지를 더해준다.

```cpp
void Image::Bloom(const float& th, const int& numRepeat, const float& weight)
{
	//https://learnopengl.com/Advanced-Lighting/Bloom

	const std::vector<Vec4> pixelsBackup = this->pixels;// 메모리 내용물까지 모두 복사

	/* Brightness가 th 보다 작은 픽셀들을 모두 검은색으로 바꾸기
	* https://en.wikipedia.org/wiki/Relative_luminance
	* Relative Luminance Y = 0.2126*R + 0.7152*G + 0.0722*B
	*/
	for (int j = 0; j < height; j ++)
		for (int i = 0; i < width; i++)
		{
			auto& c = this->GetPixel(i, j);
			const float relativeLuminance = c.v[0] * 0.2126f + c.v[1] * 0.7152f + c.v[3] * 0.0722f;

			if (relativeLuminance < th)
			{
				c.v[0] = 0.0f;
				c.v[1] = 0.0f;
				c.v[2] = 0.0f;
			}
		}

	// 여기서 Blur하지 않고 결과 확인

	// 밝은 부분만 Blur 
	for (int i = 0; i < numRepeat; i++)
	{
		this->GaussianBlur5();
	}

	// 여기서 또 한 번 결과 확인

	// 밝은 부분만 Blur한 것과 원본 이미지를 더하기 (밝은 부분 Blur에 weight 곱해서 강도 조절)
	for (int i = 0; i < pixelsBackup.size(); i++)
	{
		this->pixels[i].v[0] = std::clamp(pixels[i].v[0] * weight + pixelsBackup[i].v[0], 0.0f, 1.0f);
		this->pixels[i].v[1] = std::clamp(pixels[i].v[1] * weight + pixelsBackup[i].v[1], 0.0f, 1.0f);
		this->pixels[i].v[2] = std::clamp(pixels[i].v[2] * weight + pixelsBackup[i].v[2], 0.0f, 1.0f);
	}
}
```