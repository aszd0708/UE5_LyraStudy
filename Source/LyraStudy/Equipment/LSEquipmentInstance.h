// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LSEquipmentInstance.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class LYRASTUDY_API ULSEquipmentInstance : public UObject
{
	GENERATED_BODY()
	
public:
	ULSEquipmentInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/* 어떤 InventoryItemInstance에 의해 활성화 되었는지 (추후, QuickaBarComponent에서 보게 될 것) */
	UPROPERTY()
	TObjectPtr<UObject> Intigator;

	/* LSEquipmentDefinition에 맞게 Spawn된 Actor Instance들 */
	UPROPERTY()
	TArray<TObjectPtr<AActor>> SpawnedActor;
};
