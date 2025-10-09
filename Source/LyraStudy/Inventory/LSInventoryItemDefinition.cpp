// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/LSInventoryItemDefinition.h"

ULSInventoryItemDefinition::ULSInventoryItemDefinition(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

const ULSInventoryItemFragment* ULSInventoryItemDefinition::FindFragmentByClass(TSubclassOf<ULSInventoryItemFragment> FragmentClass) const
{
	if (FragmentClass)
	{
		// Fragments를 순회하며, IsA()를 통해 해당 클래스를 가지고 있는지 확인한다:
		for (ULSInventoryItemFragment* Fragment : Fragments)
		{
			if (Fragment && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}

    return nullptr;
}
