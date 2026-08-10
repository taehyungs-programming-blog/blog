---
layout: default
title: "03. FLyraAssetManagerStartupJob"
parent: "(Example6 - Lyra 분석)"
grand_parent: "(Unreal C++ 🚀)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌟](https://github.com/Arthur880708/LyraClone/tree/1)

---

* 뭐 하는 애인가?
	* 리소스를 미리 로드하는데 그 순서를 관리해준다.
* 우선, 어디서 사용될까?

```cpp
// 매니저 시작 시
void ULyraAssetManager::StartInitialLoading()
{
	SCOPED_BOOT_TIMING("ULyraAssetManager::StartInitialLoading");

	Super::StartInitialLoading();

    // 이렇게 매크로등록
	STARTUP_JOB(InitializeGameplayCueManager());

	{
		// 여기도 비슷한게 있네?
		STARTUP_JOB_WEIGHTED(GetGameData(), 25.f);
	}

	DoAllStartupJobs();
}
```

```cpp
// 뭔지는 아직 모르겠지만 뒤에 Weight만 다르구나?
#define STARTUP_JOB_WEIGHTED(JobFunc, JobWeight) StartupJobs.Add(FLyraAssetManagerStartupJob(#JobFunc, [this](const FLyraAssetManagerStartupJob& StartupJob, TSharedPtr<FStreamableHandle>& LoadHandle){JobFunc;}, JobWeight))
#define STARTUP_JOB(JobFunc) STARTUP_JOB_WEIGHTED(JobFunc, 1.f)
```

```cpp
struct FLyraAssetManagerStartupJob
{
	FLyraAssetManagerStartupJobSubstepProgress SubstepProgressDelegate;
	TFunction<void(const FLyraAssetManagerStartupJob&, TSharedPtr<FStreamableHandle>&)> JobFunc;
	FString JobName;
	float JobWeight;
	mutable double LastUpdate = 0;

	// JobFunc, JobName, JobWeight 를 캡쳐해 놨다가 실행하게 된다.
	FLyraAssetManagerStartupJob(const FString& InJobName, const TFunction<void(const FLyraAssetManagerStartupJob&, TSharedPtr<FStreamableHandle>&)>& InJobFunc, float InJobWeight)
		: JobFunc(InJobFunc)
		, JobName(InJobName)
		, JobWeight(InJobWeight)
	{}
```

* 실행은 어디서?

```cpp
void ULyraAssetManager::DoAllStartupJobs()
{
	SCOPED_BOOT_TIMING("ULyraAssetManager::DoAllStartupJobs");
	const double AllStartupJobsStartTime = FPlatformTime::Seconds();

	if (IsRunningDedicatedServer())
	{
		// No need for periodic progress updates, just run the jobs
		for (const FLyraAssetManagerStartupJob& StartupJob : StartupJobs)
		{
			StartupJob.DoJob();
		}
	}

    // ...
```