---
layout: default
title: "01. UE_LOG"
parent: "(Engine Source Code 자잘한 부분)"
grand_parent: "(UnrealEngine Code-Review Ver.2)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 어딜 봐야할까?

* 타고타고 들어가면 결국 아래이다.

```cpp
/** 
    * A  macro that outputs a formatted message to log if a given logging category is active at a given verbosity level
    * @param CategoryName name of the logging category
    * @param Verbosity, verbosity level to test against
    * @param Format, format text
    ***/
#define UE_LOG(CategoryName, Verbosity, Format, ...) \
{ \
    static_assert(TIsArrayOrRefOfType<decltype(Format), TCHAR>::Value, "Formatting string must be a TCHAR array."); \
    static_assert((ELogVerbosity::Verbosity & ELogVerbosity::VerbosityMask) < ELogVerbosity::NumVerbosity && ELogVerbosity::Verbosity > 0, "Verbosity must be constant and in range."); \
    CA_CONSTANT_IF((ELogVerbosity::Verbosity & ELogVerbosity::VerbosityMask) <= ELogVerbosity::COMPILED_IN_MINIMUM_VERBOSITY && (ELogVerbosity::Warning & ELogVerbosity::VerbosityMask) <= FLogCategory##CategoryName::CompileTimeVerbosity) \
    { \
        UE_LOG_EXPAND_IS_FATAL(Verbosity, PREPROCESSOR_NOTHING, if (!CategoryName.IsSuppressed(ELogVerbosity::Verbosity))) \
        { \
            auto UE_LOG_noinline_lambda = [](const auto& LCategoryName, const auto& LFormat, const auto&... UE_LOG_Args) FORCENOINLINE \
            { \
                TRACE_LOG_MESSAGE(LCategoryName, Verbosity, LFormat, UE_LOG_Args...) \
                UE_LOG_EXPAND_IS_FATAL(Verbosity, \
                    { \
                        FMsg::Logf_Internal(UE_LOG_SOURCE_FILE(__FILE__), __LINE__, LCategoryName.GetCategoryName(), ELogVerbosity::Verbosity, LFormat, UE_LOG_Args...); \
                        _DebugBreakAndPromptForRemote(); \
                        FDebug::ProcessFatalError(); \
                    }, \
                    { \
                        FMsg::Logf_Internal(nullptr, 0, LCategoryName.GetCategoryName(), ELogVerbosity::Verbosity, LFormat, UE_LOG_Args...); \
                    } \
                ) \
            }; \
            UE_LOG_noinline_lambda(CategoryName, Format, ##__VA_ARGS__); \
            UE_LOG_EXPAND_IS_FATAL(Verbosity, CA_ASSUME(false);, PREPROCESSOR_NOTHING) \
        } \
    } \
}
```

---

## 분석

```cpp
static_assert(TIsArrayOrRefOfType<decltype(Format), TCHAR>::Value, "Formatting string must be a TCHAR array.");
// TCHAR인지 확인
```

```cpp
static_assert((ELogVerbosity::Verbosity & ELogVerbosity::VerbosityMask) < ELogVerbosity::NumVerbosity && ELogVerbosity::Verbosity > 0, "Verbosity must be constant and in range.");
// arg로 들어온 Verbosity의 범위를 체크
```

```cpp
// #define CA_CONSTANT_IF(Condition) if (Condition)
CA_CONSTANT_IF((ELogVerbosity::Verbosity & ELogVerbosity::VerbosityMask) <= ELogVerbosity::COMPILED_IN_MINIMUM_VERBOSITY && (ELogVerbosity::Warning & ELogVerbosity::VerbosityMask) <= FLogCategory##CategoryName::CompileTimeVerbosity) \
// 알려야할 Verbosity인지 확인
```

```cpp
// #define UE_LOG_EXPAND_IS_FATAL(Verbosity, ActiveBlock, InactiveBlock) PREPROCESSOR_JOIN(UE_LOG_EXPAND_IS_FATAL_, Verbosity)(ActiveBlock, InactiveBlock)

```