// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LSEquipmentInstance.generated.h"

struct FLSEquipmentActorToSpawn;

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class LYRASTUDY_API ULSEquipmentInstance : public UObject
{
	GENERATED_BODY()
	
public:
	ULSEquipmentInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	* Blueprint 정의를 위한 Equip/Unequip 함수
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = Equipment, meta = (DisplayName = "OnEquipped"))
	void K2_OnEquipped();

	UFUNCTION(BlueprintImplementableEvent, Category = Equipment, meta = (DisplayName = "OnUnequipped"))
	void K2_OnUnequipped();

	UFUNCTION(BlueprintPure, Category = Equipment)
	APawn* GetPawn() const;

	void SpawnEquipmentActors(const TArray<FLSEquipmentActorToSpawn>& ActorsToSpawn);
	void DestroyEquipmentActors();

	/*
	* interfaces
	*/
	virtual void OnEquipped();
	virtual void OnUnequipped();

	/* 어떤 InventoryItemInstance에 의해 활성화 되었는지 (추후, QuickaBarComponent에서 보게 될 것) */
	UPROPERTY()
	TObjectPtr<UObject> Instigator;

	/* LSEquipmentDefinition에 맞게 Spawn된 Actor Instance들 */
	UPROPERTY()
	TArray<TObjectPtr<AActor>> SpawnedActors;
};
