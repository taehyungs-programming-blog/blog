---
layout: default
title: "12. multi material"
parent: "(DirectX12 25년 Tutorial)"
grand_parent: "(Graphics 😎)"
great_grand_parent: "Legacy Archive"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

> 하나의 object에 여러 texture를 넣어 그려보자

---

```cpp
BOOL CBasicMeshObject::InitRootSinagture()
{
	ID3D12Device5* pD3DDeivce = m_pRenderer->INL_GetD3DDevice();
	ID3DBlob* pSignature = nullptr;
	ID3DBlob* pError = nullptr;
	
    CD3DX12_DESCRIPTOR_RANGE rangesPerObj[1] = {};
	rangesPerObj[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);	// b0 : Constant Buffer View per Object

	CD3DX12_DESCRIPTOR_RANGE rangesPerTriGroup[1] = {};
	rangesPerTriGroup[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);	// t0 : Shader Resource View(Tex) per Tri-Group

    // root param을 이렇게 배열로 넘긴다
	CD3DX12_ROOT_PARAMETER rootParameters[2] = {};
	rootParameters[0].InitAsDescriptorTable(_countof(rangesPerObj), rangesPerObj, D3D12_SHADER_VISIBILITY_ALL);
	rootParameters[1].InitAsDescriptorTable(_countof(rangesPerTriGroup), rangesPerTriGroup, D3D12_SHADER_VISIBILITY_ALL);

    /*
    * 쓸때는 이렇게 쓸 예정
        * | CBV - RootParam(0) | - 이건 한 번만 쓸것이기에 고정
        * | SRV[0] - RootParam(1) | 
        * | SRV[1] - RootParam(1) | 
        * ...
        * 여기서 RootParam(1)을 계속 쓰면서, Draw콜을 여러번 할 예정이다.

        * | CBV - RootParam(0) | SRV[0] - RootParam(1) |  -> 이런식으로 descriptor를 만든다고 생각하자
        * | CBV - RootParam(0) | SRV[1] - RootParam(1) |
    */

	// ...

	// Create an empty root signature.
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	//rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	rootSignatureDesc.Init(_countof(rootParameters), rootParameters, 1, &sampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	if (FAILED(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pSignature, &pError)))
	{
        /* (참고)
        루트 시그니처 구조체를 바이너리 데이터로 변환합니다. 이 함수는 다음과 같은 과정을 수행합니다:
        입력: rootSignatureDesc (루트 시그니처 설명 구조체)를 받아서
        처리: 이를 GPU가 이해할 수 있는 바이너리 형태로 직렬화
        */
		__debugbreak();
	}

	if (FAILED(pD3DDeivce->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), IID_PPV_ARGS(&m_pRootSignature))))
	{
		__debugbreak();
	}
	if (pSignature)
	{
		pSignature->Release();
		pSignature = nullptr;
	}
	if (pError)
	{
		pError->Release();
		pError = nullptr;
	}
	return TRUE;
}
```

* 그럼 Draw는 어떻게 동작할까?
* 우선 MeshObject를 만드는 곳 부터 보자.

```cpp
void* CreateBoxMeshObject()
{
	void* pMeshObj = nullptr;

	// create box mesh
	// create vertices and indices
	WORD	pIndexList[36] = {};
	BasicVertex* pVertexList = nullptr;
	DWORD dwVertexCount = CreateBoxMesh(&pVertexList, pIndexList, (DWORD)_countof(pIndexList), 0.25f);

	// create BasicMeshObject from Renderer
	pMeshObj = g_pRenderer->CreateBasicMeshObject();

	const WCHAR* wchTexFileNameList[6] =
	{
		L"tex_00.dds",
		L"tex_01.dds",
		L"tex_02.dds",
		L"tex_03.dds",
		L"tex_04.dds",
		L"tex_05.dds"
	};

	// Set meshes to the BasicMeshObject
	g_pRenderer->BeginCreateMesh(pMeshObj, pVertexList, dwVertexCount, 6);	// 박스의 6면-1면당 삼각형 2개-인덱스 6개
	for (DWORD i = 0; i < 6; i++)
	{
		g_pRenderer->InsertTriGroup(pMeshObj, pIndexList + i * 6, 2, wchTexFileNameList[i]);
	}
	g_pRenderer->EndCreateMesh(pMeshObj);

	// delete vertices and indices
	if (pVertexList)
	{
		DeleteBoxMesh(pVertexList);
		pVertexList = nullptr;
	}
	return pMeshObj;
}
```

```cpp
void CBasicMeshObject::Draw(ID3D12GraphicsCommandList* pCommandList, const XMMATRIX* pMatWorld)
{
	ID3D12Device5* pD3DDeivce = m_pRenderer->INL_GetD3DDevice();
	UINT srvDescriptorSize = m_pRenderer->INL_GetSrvDescriptorSize();
	CDescriptorPool* pDescriptorPool = m_pRenderer->INL_GetDescriptorPool();
	ID3D12DescriptorHeap* pDescriptorHeap = pDescriptorPool->INL_GetDescriptorHeap();
	CSimpleConstantBufferPool* pConstantBufferPool = m_pRenderer->INL_GetConstantBufferPool();
	

    // Draw 함수는 멀티 머티리얼(여러 텍스처를 가진) 메쉬 객체를 렌더링하는 함수
    // 핵심은 하나의 메쉬가 여러 개의 TriGroup(삼각형 그룹)으로 구성되어 있고, 각 TriGroup마다 다른 텍스처를 사용
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescriptorTable = {};
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescriptorTable = {};
	DWORD dwRequiredDescriptorCount = DESCRIPTOR_COUNT_PER_OBJ + (m_dwTriGroupCount * DESCRIPTOR_COUNT_PER_TRI_GROUP);

	if (!pDescriptorPool->AllocDescriptorTable(&cpuDescriptorTable, &gpuDescriptorTable, dwRequiredDescriptorCount))
	{
		__debugbreak();
	}

	// 각각의 draw()에 대해 독립적인 constant buffer(내부적으로는 같은 resource의 다른 영역)를 사용한다.
	CB_CONTAINER* pCB = pConstantBufferPool->Alloc();
	if (!pCB)
	{
		__debugbreak();
	}

	// ...

	// per Obj
	CD3DX12_CPU_DESCRIPTOR_HANDLE Dest(cpuDescriptorTable, BASIC_MESH_DESCRIPTOR_INDEX_PER_OBJ_CBV, srvDescriptorSize);
	pD3DDeivce->CopyDescriptorsSimple(1, Dest, pCB->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	Dest.Offset(1, srvDescriptorSize);

	// per tri-group
	for (DWORD i = 0; i < m_dwTriGroupCount; i++)
	{
		INDEXED_TRI_GROUP* pTriGroup = m_pTriGroupList + i;
		TEXTURE_HANDLE* pTexHandle = pTriGroup->pTexHandle;
		if (pTexHandle)
		{
			pD3DDeivce->CopyDescriptorsSimple(1, Dest, pTexHandle->srv, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		}
		else
		{
			__debugbreak();
		}
        // | CBV (Object) | SRV (TriGroup 0) | SRV (TriGroup 1) | SRV (TriGroup 2) | ...
		Dest.Offset(1, srvDescriptorSize);
	}
	
	// set RootSignature
	pCommandList->SetGraphicsRootSignature(m_pRootSignature);
	pCommandList->SetDescriptorHeaps(1, &pDescriptorHeap);

	// ex) when TriGroupCount = 3
	// per OBJ | TriGroup 0 | TriGroup 1 | TriGroup 2 |
	// CBV     |     SRV    |     SRV    |     SRV    | 

	pCommandList->SetPipelineState(m_pPipelineState);
	pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pCommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);

	// set descriptor table for root-param 0
	pCommandList->SetGraphicsRootDescriptorTable(0, gpuDescriptorTable);	// Entry per Obj

	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescriptorTableForTriGroup(gpuDescriptorTable, DESCRIPTOR_COUNT_PER_OBJ, srvDescriptorSize);
	for (DWORD i = 0; i < m_dwTriGroupCount; i++)
	{
		// set descriptor table for root-param 1
		pCommandList->SetGraphicsRootDescriptorTable(1, gpuDescriptorTableForTriGroup);	// Entry of Tri-Groups
		gpuDescriptorTableForTriGroup.Offset(1, srvDescriptorSize);

		INDEXED_TRI_GROUP* pTriGroup = m_pTriGroupList + i;
		pCommandList->IASetIndexBuffer(&pTriGroup->IndexBufferView);
		pCommandList->DrawIndexedInstanced(pTriGroup->dwTriCount * 3, 1, 0, 0, 0);
	}
}
```