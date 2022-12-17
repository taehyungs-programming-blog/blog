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

## Guassian Filter 넣기

```cpp

```