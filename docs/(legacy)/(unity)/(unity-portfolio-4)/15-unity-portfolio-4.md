---
layout: default
title: "15. Unity Shader"
parent: "(포트폴리오 4 - 방치형 게임)"
grand_parent: "(Unity ✨)"
great_grand_parent: "Legacy Archive"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unity.IncrementalGame.Example/tree/6)

---

* 모두 볼 필요는 없을 듯 하며 아래 코드를 대표예시로 보자.

```csharp
Shader "SkillIndicator/Circle" 
{
    Properties 
    {
        // 여기 Property 넣음
    }

    SubShader 
    {
        // ...
        // 간략한 내용은 생략

            // vertex 함수를 Vertex Shader라 생각해면 된다.
            Varyings vert(Attributes v) 
            {
                Varyings o = (Varyings)0;
                o.uv = v.texcoord;
                o.positionCS = TransformObjectToHClip(v.positionOS.xyz);
                return o;
            }

            // frag 함수를 Fragment Shader(Pixel Shader)라 생각해면 된다.
            float4 frag(Varyings i) : SV_Target 
            {
                half4 col = 0;
                half2 uv = i.uv;

                // 텍스처 샘플링을 통해 픽셀 색상 얻어옴.
                half4 mainTex = SAMPLE_TEXTURE2D(_MainTex, sampler_MainTex, uv);
                mainTex *= _Intensity;

                #if _INDICATOR_ON
                    return mainTex.b * 0.6 * _Color;
                #endif

                // 중앙을 기준으로 좌표 재계산 uv (0~1) -> center (-1~1)
                float2 centerUV = (uv * 2 - 1);

                // atan2(x,y) : x/y에 해당하는 각도를 리턴한다.
                    // 리턴된 각도는 [-π, π]의 범위이다.
                    /*
                           0
                           |
                           |
                    ----------------
                           |
                           |
                        -π or π
                    */
                // atan2(centerUV.g, centerUV.r)/3.14 == [-1, 1]
                // abs(atan2(centerUV.g, centerUV.r)/3.14) == [0, 1] 여기서 abs를 해버리기에 좌우 대칭의 구조가 탄생
                // 1-abs(atan2(centerUV.g, centerUV.r)/3.14) == [0, 1] 의 범위를 갖는다
                float atan2UV = 1-abs(atan2(centerUV.g, centerUV.r)/3.14);

                // _Sector : 외부에서 지정
                // ceil(x) : 올림
                // lerp(x,y,s) : 선형보간인 x + s(y - x) 를 리턴한다.
                    // lerp(1, 0, 1) == 0
                    // lerp(1, 0, 0) == 1
                // Angle (0~360) * 0.002778 -> (0~1)
                // sector == 1 그리고 0일시 안그린다.
                half sector = lerp(1.0, 1.0 - ceil(atan2UV - _Angle * 0.002777778), _Sector);
                
                // sector랑 비슷하지만 조금 더 큰 각도
                half sectorBig = lerp(1.0, 1.0 - ceil(atan2UV - (_Angle + _Outline) * 0.002777778), _Sector);

                // 경계선을 판별.
                half outline = (sectorBig - sector) * mainTex.g * _OutlineAlpha;

                // step(x,y) : x≤y 이면 1을 리턴하고, 그렇지 않으면 0을 리턴한다.
                // needOutline : 0 or 1
                half needOutline = 1 - step(359, _Angle);
                outline *= needOutline;

                col = mainTex.r * _Color * sector + outline * _Color;

                // smoothstep(min,max,x) : x가 [min, max] 사이의 값인 경우에 대해서 [0, 1] 사이에서 부드럽게 변하는 Hermite 보간법
                half flowCircleInner = smoothstep(_Duration - _FlowFade, _Duration, length(centerUV));

                // step(x,y) : x≤y 이면 1을 리턴하고, 그렇지 않으면 0을 리턴한다.
                // length(x) : 벡터의 길이를 계산한다.
                half flowCircleMask = step(length(centerUV), _Duration);

                // 결론 : flow는 안쪽 빨간색의 색상을 더해준다.
                half4 flow = flowCircleInner * flowCircleMask * _FlowColor * mainTex.g * sector;

                // 최종 색상 도출
                col += flow;

                return col;
            }
            ENDHLSL
        }
    }
```

