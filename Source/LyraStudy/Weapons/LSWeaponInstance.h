// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Cosmetics/LSCosmeticAnimationTypes.h"
#include "Equipment/LSEquipmentInstance.h"
#include "LSWeaponInstance.generated.h"

/**
 * 
 */
UCLASS()
class LYRASTUDY_API ULSWeaponInstance : public ULSEquipmentInstance
{
	GENERATED_BODY()
	
public:
	ULSWeaponInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/* Weapon에 Equip/Unequip에 대한 Animation Set 정보를 들고 있다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	FLSAnimLayerSelectionSet EquippedAnimSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	FLSAnimLayerSelectionSet UnequippedAnimSet;
};
