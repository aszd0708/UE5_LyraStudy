// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LSInventoryManagerComponent.generated.h"

class ULSInventoryItemInstance;

/** Inventory Item 단위 객체 */
USTRUCT(BlueprintType)
struct FLSInventoryEntry
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<ULSInventoryItemInstance> Instance = nullptr;
};

/* Inventory Item 관리 객체 */
USTRUCT(BlueprintType)
struct FLSInventoryList
{
	GENERATED_BODY()

	FLSInventoryList(UActorComponent* InOwnerComponent = nullptr) : OwnerComponent(InOwnerComponent)
	{
	}

	UPROPERTY()
	TArray<FLSInventoryEntry> Entries;

	UPROPERTY()
	TObjectPtr<UActorComponent> OwnerComponent;
};

/**
 * PlayerController의 Component로서 Inventory를 관리한다
 * - 사실 UActorComponent 상속이 아닌 UControllerComponent를 상속받아도 될거 같은데... 일단 Lyra 기준으로 UActorComponent를 상속받고 있다
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LYRASTUDY_API ULSInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	ULSInventoryManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY()
	FLSInventoryList InventoryList;
};
