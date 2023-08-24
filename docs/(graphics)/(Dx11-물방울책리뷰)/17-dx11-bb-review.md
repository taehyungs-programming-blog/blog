---
layout: default
title: "17. Picking"
parent: "(DirectX11 물방울책 리뷰)"
grand_parent: "(Graphics 😎)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/DirectX11-3d-tutorials/tree/15)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/bb/bb-17-1.png"/>
</p>

* 우클릭시 Vertex에 색을 입혀보자

```cpp
void PickingDemo::Pick(int32 sx, int32 sy)
{
	XMMATRIX P = _camera.Proj();

	Matrix m = P;
	// Compute picking ray in view space.
	float vx = (+2.0f * sx / _clientWidth - 1.0f) / m(0, 0); // P(0, 0);
	float vy = (-2.0f * sy / _clientHeight + 1.0f) / m(1, 1); // P(1, 1);

	// Ray definition in view space.
	XMVECTOR rayOrigin = ::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR rayDir = ::XMVectorSet(vx, vy, 1.0f, 0.0f);

	// Tranform ray to local space of Mesh.
	XMMATRIX V = _camera.View();
	XMVECTOR D1 = ::XMMatrixDeterminant(V);
	XMMATRIX invView = ::XMMatrixInverse(&D1, V);

	XMMATRIX W = ::XMLoadFloat4x4(&_meshWorld);
	XMVECTOR D2 = ::XMMatrixDeterminant(W);
	XMMATRIX invWorld = ::XMMatrixInverse(&D2, W);

	XMMATRIX toLocal = ::XMMatrixMultiply(invView, invWorld);

    // XMVector3TransformCoord - 좌표를 반영해서 toLocal로
    // XMVector3TransformNormal - 좌표는 반영말고 방향만 toLocal로
	rayOrigin = ::XMVector3TransformCoord(rayOrigin, toLocal);
	rayDir = ::XMVector3TransformNormal(rayDir, toLocal);

	// Make the ray direction unit length for the intersection tests.
	rayDir = ::XMVector3Normalize(rayDir);

	// If we hit the bounding box of the Mesh, then we might have picked a Mesh triangle,
	// so do the ray/triangle tests.
	//
	// If we did not hit the bounding box, then it is impossible that we hit 
	// the Mesh, so do not waste effort doing ray/triangle tests.

	// Assume we have not picked anything yet, so init to -1.
	_pickedTriangle = -1;
	float tmin = 0.0f;

	if (_meshBox.Intersects(rayOrigin, rayDir, tmin))
	{
		// Find the nearest ray/triangle intersection.
		tmin = MathHelper::Infinity;
		for (UINT i = 0; i < _meshIndices.size() / 3; ++i)
		{
			// Indices for this triangle.
			UINT i0 = _meshIndices[i * 3 + 0];
			UINT i1 = _meshIndices[i * 3 + 1];
			UINT i2 = _meshIndices[i * 3 + 2];

			// Vertices for this triangle.
			XMVECTOR v0 = ::XMLoadFloat3(&_meshVertices[i0].pos);
			XMVECTOR v1 = ::XMLoadFloat3(&_meshVertices[i1].pos);
			XMVECTOR v2 = ::XMLoadFloat3(&_meshVertices[i2].pos);

			// We have to iterate over all the triangles in order to find the nearest intersection.
			float t = 0.0f;

			if (TriangleTests::Intersects(rayOrigin, rayDir, v0, v1, v2, t))
			{
                // 거리를 반영해 가장가까운애를 찾는다
				if (t < tmin)
				{
					// This is the new nearest picked triangle.
					tmin = t;
					_pickedTriangle = i;
				}
			}
		}
	}
}
```
