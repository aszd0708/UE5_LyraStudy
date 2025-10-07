// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LSInventoryItemDefinition.generated.h"

/**
 * ItenInventry에 대한 Franament는 이해하기 힘들수 있음
 * - Lyra에서 사용하는 예시:
 *	- ULyraInventoryFragment_EquippableItem은 EquipmentItemDefinition을 가지고 있으며, 장착 가능한 아이템을 의미한다.
 *	- ULyraInventoryFragment_SetStats는 아이템에 대한 정보를 가지고 있다.
 *		- Rifle에 대한 SetStats으로 총알(Ammo)에 대한 장착 최대치와 현재 남은 잔탄 수를 예시로 들 수 있다.
 */
UCLASS(Abstract, DefaultToinstanced, EditInlineNew)
class ULSInventoryItemFragment : public UObject
{
	GENERATED_BODY()
public:
};


UCLASS(Blueprintable)
class LYRASTUDY_API ULSInventoryItemDefinition : public UObject
{
	GENERATED_BODY()
	
public:
	ULSInventoryItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	const ULSInventoryItemFragment* FindFragmentByClass(TSubclassOf<ULSInventoryItemFragment> FragmentClass) const;

	/** Inventory Item 정의(메타) 이름 **/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Display)
	FText DisplayName;

	/** Inventory Item의 Component를 Fragment로 인식하면 된다 */
	UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = Display)
	TArray<TObjectPtr<ULSInventoryItemFragment>> Fragments;
};
