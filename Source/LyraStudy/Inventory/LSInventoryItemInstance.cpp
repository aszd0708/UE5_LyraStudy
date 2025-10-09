// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/LSInventoryItemInstance.h"
#include "Inventory/LSInventoryItemDefinition.h"

ULSInventoryItemInstance::ULSInventoryItemInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

const ULSInventoryItemFragment* ULSInventoryItemInstance::FindFragmentByClass(TSubclassOf<ULSInventoryItemFragment> FragmentClass) const
{
	if ((ItemDef != nullptr) && (FragmentClass != nullptr))
	{
		// HakInventoryItemDefinition은 모든 멤버 변수가 EditDefaultsOnly로 선언되어 있으므로, GetDefault로 가져와도 무관하다
		// - Fragment 정보는 Instance가 아닌 Definition에 있다
		return GetDefault<ULSInventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}

	return nullptr;
}
