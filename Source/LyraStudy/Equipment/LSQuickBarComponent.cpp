// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/LSQuickBarComponent.h"

ULSQuickBarComponent::ULSQuickBarComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void ULSQuickBarComponent::BeginPlay()
{
	// NumSlots에 따라 미리 Slots를 할당한다.
	if (Slots.Num() < NumSlots)
	{
		Slots.AddDefaulted(NumSlots - Slots.Num());
	}

	Super::BeginPlay();
}

void ULSQuickBarComponent::AddItemToSlot(int32 SlotIndex, ULSInventoryItemInstance* Item)
{
	// 해당 로직을 보면, Slots는 Add로 동적 추가가 아닌, Index 에 바로 넣는다.
	// - 그럼 미리 Pre-size 했다는 것인데 이는 BeginPlay()에서 진행한다.
	if (Slots.IsValidIndex(SlotIndex) && (Item != nullptr))
	{
		if (Slots[SlotIndex] == nullptr)
		{
			Slots[SlotIndex] = Item;
		}
	}
}
