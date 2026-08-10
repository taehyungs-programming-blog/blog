---
layout: default
title: "07. Composition"
parent: "(문법)"
grand_parent: "(Unreal C++ 🚀)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unreal_Cpp_basic/tree/main/6.Composition)

## Composition?

* 객체 지향 설계에서 **상속이 가진 Is-A** 관계만 의존해서는 설계와 유지보수가 어렵다
* Composition은 객체 지향 설계에서 **Has-A** 관계를 구현하는 설계 방법

* 예를 들자면 

```cpp
class Card
{
public:
    Card(int InId) : Id(InId) {}
    int Id = 0;
};

class Person
{
public:
    Person(Card InCard) : IdCard(InCard) {}

protected:
    // Has-A : 소유관계라 생각하면 된다.
    Card IdCard;
};
```

---

## Unreal Example

```cpp
UENUM()
enum class ECardType : uint8
{
	Student = 1 UMETA(DisplayName = "For Student"),
	Teacher UMETA(DisplayName = "For Teacher"),
	Staff UMETA(DisplayName = "For Staff"),
	Invalid
};

/**
 * 
 */
UCLASS()
class UNREALCOMPOSITION_API UCard : public UObject
{
	GENERATED_BODY()
public:
	UCard();

	ECardType GetCardType() const { return CardType; }
	void SetCardType(ECardType InCardType) { CardType = InCardType; }

private:
	UPROPERTY()
	ECardType CardType;

	UPROPERTY()
	uint32 Id;	
};
```

```cpp
void UMyGameInstance::Init()
{
	Super::Init();

	UE_LOG(LogTemp, Log, TEXT("============================"));
	TArray<UPerson*> Persons = { NewObject<UStudent>(), NewObject<UTeacher>(), NewObject<UStaff>() };
	for (const auto Person : Persons)
	{
		const UCard* OwnCard =Person->GetCard();
		check(OwnCard);
		ECardType CardType = OwnCard->GetCardType();
		//UE_LOG(LogTemp, Log, TEXT("%s님이 소유한 카드 종류 %d"), *Person->GetName(), CardType);

		const UEnum* CardEnumType = FindObject<UEnum>(nullptr, TEXT("/Script/UnrealComposition.ECardType"));
		if (CardEnumType)
		{
			FString CardMetaData = CardEnumType->GetDisplayNameTextByValue((int64)CardType).ToString();
			UE_LOG(LogTemp, Log, TEXT("%s님이 소유한 카드 종류 %s"), *Person->GetName(), *CardMetaData);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("============================"));
}
```