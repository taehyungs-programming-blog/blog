---
layout: default
title: "15. text rendering"
parent: "(DirectX12 25년 Tutorial)"
grand_parent: "(Graphics 😎)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 방향

* Text 출력을 API를 통해 Text to bitmap
* bitmap을 texture에 업데이트
* Dynamic Texture + Sprite 출력을 통해 화면 출

> GDI 또는 Direct2D/DirectWrite 등을 활용해 보자.

---

## 시작

```cpp
BOOL CFontManager::CreateD2D(ID3D12Device* pD3DDevice, ID3D12CommandQueue* pCommandQueue, BOOL bEnableDebugLayer)
{
	UINT	d3d11DeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	D2D1_FACTORY_OPTIONS d2dFactoryOptions = {};

	ID2D1Factory3*	pD2DFactory = nullptr;
	ID3D11Device*	pD3D11Device = nullptr;
	ID3D11DeviceContext*	pD3D11DeviceContext = nullptr;
	ID3D11On12Device*		pD3D11On12Device = nullptr;
	if (bEnableDebugLayer)
	{
		d3d11DeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	}
	d2dFactoryOptions.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;

	if (FAILED(D3D11On12CreateDevice(pD3DDevice,
		d3d11DeviceFlags,
		nullptr,
		0,
		(IUnknown**)&pCommandQueue,
		1,
		0,
		&pD3D11Device,
		&pD3D11DeviceContext,
		nullptr
		)))
	{
		__debugbreak();
	}
	if (FAILED(pD3D11Device->QueryInterface(IID_PPV_ARGS(&pD3D11On12Device))))
		__debugbreak();

	// Create D2D/DWrite components.
	D2D1_DEVICE_CONTEXT_OPTIONS deviceOptions = D2D1_DEVICE_CONTEXT_OPTIONS_NONE;
	if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), &d2dFactoryOptions, (void**)&pD2DFactory)))
	{
		__debugbreak();
	}

	IDXGIDevice*	pDXGIDevice = nullptr;
	if (FAILED(pD3D11On12Device->QueryInterface(IID_PPV_ARGS(&pDXGIDevice))))
	{
		__debugbreak();
	}
	if (FAILED(pD2DFactory->CreateDevice(pDXGIDevice, &m_pD2DDevice)))
	{
		__debugbreak();
	}


	if (FAILED(m_pD2DDevice->CreateDeviceContext(deviceOptions, &m_pD2DDeviceContext)))
	{
		__debugbreak();
	}

    // ...
```

```cpp
BOOL CFontManager::WriteTextToBitmap(BYTE* pDestImage, UINT DestWidth, UINT DestHeight, UINT DestPitch, int* piOutWidth, int* piOutHeight, FONT_HANDLE* pFontHandle, const WCHAR* wchString, DWORD dwLen)
{
	int iTextWidth = 0;
	int iTextHeight = 0;

	BOOL bResult = CreateBitmapFromText(&iTextWidth, &iTextHeight, pFontHandle->pTextFormat, wchString, dwLen);
	if (bResult)
	{
		if (iTextWidth > (int)DestWidth)
			iTextWidth = (int)DestWidth;

		if (iTextHeight > (int)DestHeight)
			iTextHeight = (int)DestHeight;

		D2D1_MAPPED_RECT	mappedRect;
		if (FAILED(m_pD2DTargetBitmapRead->Map(D2D1_MAP_OPTIONS_READ, &mappedRect)))
			__debugbreak();

		BYTE*	pDest = pDestImage;
		char*	pSrc = (char*)mappedRect.bits;

		for (DWORD y = 0; y < (DWORD)iTextHeight; y++)
		{
			memcpy(pDest, pSrc, iTextWidth * 4);
			pDest += DestPitch;
			pSrc += mappedRect.pitch;
		}
		m_pD2DTargetBitmapRead->Unmap();
	}
	*piOutWidth = iTextWidth;
	*piOutHeight = iTextHeight;
	return bResult;
}

BOOL CFontManager::CreateBitmapFromText(int* piOutWidth, int* piOutHeight, IDWriteTextFormat* pTextFormat, const WCHAR* wchString, DWORD dwLen)
{
	BOOL	bResult = FALSE;

	ID2D1DeviceContext*	pD2DDeviceContext = m_pD2DDeviceContext;
	IDWriteFactory5*	pDWFactory = m_pDWFactory;
	D2D1_SIZE_F max_size = pD2DDeviceContext->GetSize();
	max_size.width = (float)m_D2DBitmapWidth;
	max_size.height = (float)m_D2DBitmapHeight;

	IDWriteTextLayout*	pTextLayout = nullptr;
	if (pDWFactory && pTextFormat)
	{
		if (FAILED(pDWFactory->CreateTextLayout(wchString, dwLen, pTextFormat, max_size.width, max_size.height, &pTextLayout)))
			__debugbreak();
	}
	DWRITE_TEXT_METRICS metrics = {};
	if (pTextLayout)
	{
		pTextLayout->GetMetrics(&metrics);

		// 타겟설정
		pD2DDeviceContext->SetTarget(m_pD2DTargetBitmap);

		// 안티앨리어싱모드 설정
		pD2DDeviceContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

		// 텍스트 렌더링
		pD2DDeviceContext->BeginDraw();

		pD2DDeviceContext->Clear(D2D1::ColorF(D2D1::ColorF::Black));
		pD2DDeviceContext->SetTransform(D2D1::Matrix3x2F::Identity());

		pD2DDeviceContext->DrawTextLayout(D2D1::Point2F(0.0f, 0.0f), pTextLayout, m_pWhiteBrush);

		// We ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
		// is lost. It will be handled during the next call to Present.
		pD2DDeviceContext->EndDraw();

		// 안티앨리어싱 모드 복구    
		pD2DDeviceContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_DEFAULT);
		pD2DDeviceContext->SetTarget(nullptr);

		// 레이아웃 오브젝트 필요없으니 Release
		pTextLayout->Release();
		pTextLayout = nullptr;
	}
	int width = (int)ceil(metrics.width);
	int height = (int)ceil(metrics.height);

	D2D1_POINT_2U	destPos = {};
	D2D1_RECT_U		srcRect = { 0, 0, width, height };
	if (FAILED(m_pD2DTargetBitmapRead->CopyFromBitmap(&destPos, m_pD2DTargetBitmap, &srcRect)))
		__debugbreak();

	*piOutWidth = width;
	*piOutHeight = height;

	bResult = TRUE;

lb_return:
	return bResult;
}
```

```cpp
void CD3D12Renderer::UpdateTextureWithImage(void* pTexHandle, const BYTE* pSrcBits, UINT SrcWidth, UINT SrcHeight)
{
	TEXTURE_HANDLE* pTextureHandle = (TEXTURE_HANDLE*)pTexHandle;
	ID3D12Resource* pDestTexResource = pTextureHandle->pTexResource;
	ID3D12Resource* pUploadBuffer = pTextureHandle->pUploadBuffer;

	D3D12_RESOURCE_DESC Desc = pDestTexResource->GetDesc();
	if (SrcWidth > Desc.Width)
	{
		__debugbreak();
	}
	if (SrcHeight > Desc.Height)
	{
		__debugbreak();
	}
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT Footprint;
	UINT	Rows = 0;
	UINT64	RowSize = 0;
	UINT64	TotalBytes = 0;

	m_pD3DDevice->GetCopyableFootprints(&Desc, 0, 1, 0, &Footprint, &Rows, &RowSize, &TotalBytes);

	BYTE*	pMappedPtr = nullptr;
	CD3DX12_RANGE readRange(0, 0);

	HRESULT hr = pUploadBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pMappedPtr));
	if (FAILED(hr))
		__debugbreak();

	const BYTE* pSrc = pSrcBits;
	BYTE* pDest = pMappedPtr;
	for (UINT y = 0; y < SrcHeight; y++)
	{
		memcpy(pDest, pSrc, SrcWidth * 4);
		pSrc += (SrcWidth * 4);
		pDest += Footprint.Footprint.RowPitch;
	}
	// Unmap
	pUploadBuffer->Unmap(0, nullptr);

	pTextureHandle->bUpdated = TRUE;
}
```