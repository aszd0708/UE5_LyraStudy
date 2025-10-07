// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/LSInventoryManagerComponent.h"
#include "Inventory/LSInventoryItemDefinition.h"
#include "Inventory/LSInventoryItemInstance.h"



ULSInventoryItemInstance* FLSInventoryList::AddEntry(TSubclassOf<ULSInventoryItemDefinition> ItemDef)
{
	ULSInventoryItemInstance* Result = nullptr;
	check(ItemDef);
	check(OwnerComponent);

	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	FLSInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = NewObject<ULSInventoryItemInstance>(OwningActor);
	NewEntry.Instance->ItemDef = ItemDef;

	Result = NewEntry.Instance;
	return Result;
}

ULSInventoryManagerComponent::ULSInventoryManagerComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), InventoryList(this)
{
}

ULSInventoryItemInstance* ULSInventoryManagerComponent::AddItemDefinition(TSubclassOf<ULSInventoryItemDefinition> ItemDef)
{
	ULSInventoryItemInstance* Result = nullptr;
	if (ItemDef)
	{
		Result = InventoryList.AddEntry(ItemDef);
	}
	return Result;
}