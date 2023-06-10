---
layout: default
title: "10. UI 개선"
parent: "(포트폴리오 3 - 뱀서 모작)"
grand_parent: "(Unity ✨)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* 기존 코드의 복습에 가깝다
    * Event Bind부분은 다름!

```csharp
public class UI_GameResultPopup : UI_Base
{
    enum GameObjects
    {
        ContentObject,
        ResultRewardScrollContentObject,
        ResultGoldObject,
        ResultKillObject,
    }

    enum Texts
    {
        GameResultPopupTitleText,
        ResultStageValueText,
        ResultSurvivalTimeText,
        ResultSurvivalTimeValueText,
        ResultGoldValueText,
        ResultKillValueText,
        ConfirmButtonText,
    }

    enum Buttons
    {
        StatisticsButton,
        ConfirmButton,
    }

    public override bool Init()
    {
        if (base.Init() == false)
            return false;

        BindObject(typeof(GameObjects));
        BindText(typeof(Texts));
        BindButton(typeof(Buttons));

        GetButton((int)Buttons.StatisticsButton).gameObject.BindEvent(OnClickStatisticsButton);
        GetButton((int)Buttons.ConfirmButton).gameObject.BindEvent(OnClickConfirmButton);

        RefreshUI();
        return true;
    }

    public void SetInfo()
    {
        RefreshUI();
    }

    void RefreshUI()
    {
        if (_init == false)
            return;

        // 정보 취합
        GetText((int)Texts.GameResultPopupTitleText).text = "Game Result";
        GetText((int)Texts.ResultStageValueText).text = "4 STAGE";
        GetText((int)Texts.ResultSurvivalTimeText).text = "Survival Time";
        GetText((int)Texts.ResultSurvivalTimeValueText).text = "14:23";
        GetText((int)Texts.ResultGoldValueText).text = "200";
        GetText((int)Texts.ResultKillValueText).text = "100";
        GetText((int)Texts.ConfirmButtonText).text = "OK";
    }

    void OnClickStatisticsButton()
    {
        Debug.Log("OnClickStatisticsButton");
    }

    void OnClickConfirmButton()
    {
		Debug.Log("OnClickConfirmButton");
	}
}
```