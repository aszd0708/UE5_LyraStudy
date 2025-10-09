// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/LSQuickBarComponent.h"
#include "Equipment/LSEquipmentDefinition.h"
#include "Equipment/LSEquipmentInstance.h"
#include "Equipment/LSEquipementManagerComponent.h"
#include "Inventory/LSInventoryFragment_EquippableItem.h"
#include "Inventory/LSInventoryItemInstance.h"

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

ULSEquipementManagerComponent* ULSQuickBarComponent::FindEquipmentManager() const
{
	if (AController* OwnerController = Cast<AController>(GetOwner()))
	{
		if (APawn* Pawn = OwnerController->GetPawn())
		{
			return Pawn->FindComponentByClass<ULSEquipementManagerComponent>();
		}
	}

	return nullptr;
}

void ULSQuickBarComponent::UnequipItemInSlot()
{	
	// 참고로 QuickBar는 Controller에 붙어있는 Component이지만, EquipmentManagerComponent는 Controller가 소유(Possess)하고 있는 Pawn의 Component이다
	if (ULSEquipementManagerComponent* EquipmentManager = FindEquipmentManager())
	{
		// 현재 장착된 Item에 있다면,
		if (EquippedItem)
		{
			// EquipementManager를 통해, Pawn의 장비를 해제시킨다
			EquipmentManager->UnequipItem(EquippedItem);

			// 그리고 Controller에도 EquipItem의 상태를 없는 것으로 업데이트한다
			EquippedItem = nullptr;
		}
	}
}

void ULSQuickBarComponent::EquipItemInSlot()
{
	check(Slots.IsValidIndex(ActiveSlotIndex));
	check(EquippedItem == nullptr);

	// 현재 활성화된 ActiveSlotIndex를 활용하여 활성화된 InventoryItemInstance를 찾는다
	if (ULSInventoryItemInstance* SlotItem = Slots[ActiveSlotIndex])
	{
		// Slot Item을 통해 (InventoryItemInstance) InventoryFragment_EquippableItem의 Fragment를 찾는다:
		// - 찾는 것이 실패했다면, 장착할 수 없는 Inventory Item임을 의미한다
		if (const ULSInventoryFragment_EquippableItem* EquipInfo = SlotItem->FindFragmentByClass<ULSInventoryFragment_EquippableItem>())
		{
			// EquippableItem에서 EquipmentDefinition을 찾는다:
			// - EquipmentDefinition이 있어야, 장착할 수 있다
			TSubclassOf<ULSEquipmentDefinition> EquipDef = EquipInfo->EquipmentDefinition;
			if (EquipDef)
			{
				// 아래는 Unequip이랑 비슷하게 EquipmentManager를 통해 장착한다
				if (ULSEquipementManagerComponent* EquipmentManager = FindEquipmentManager())
				{
					EquippedItem = EquipmentManager->EquipItem(EquipDef);

					if (EquippedItem)
					{
						// EquippedItem에는 앞서 보았던 Instiagor로 Slot을 대상으로 넣는다
						EquippedItem->Instigator = SlotItem;
					}
				}
			}
		}
	}
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

void ULSQuickBarComponent::SetActivateSlotIndex(int32 NewIndex)
{
	if (Slots.IsValidIndex(NewIndex) && (ActiveSlotIndex != NewIndex))
	{
		// UnequipItem/EquipItem을 통해, NewIndex를 통해 할당된 Item을 장착 및 업데이트를 진행한다.
		UnequipItemInSlot();
		ActiveSlotIndex = NewIndex;
		EquipItemInSlot();
	}
}
