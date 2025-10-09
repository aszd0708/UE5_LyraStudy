// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LSInventoryItemInstance.generated.h"

class ULSInventoryItemDefinition;
class ULSInventoryItemFragment;

/**
 * 
 */
UCLASS()
class LYRASTUDY_API ULSInventoryItemInstance : public UObject
{
	GENERATED_BODY()

public:
	ULSInventoryItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	const ULSInventoryItemFragment* FindFragmentByClass(TSubclassOf<ULSInventoryItemFragment> FragmentClass) const;

	template <typename ResultClass>
	const ResultClass* FindFragmentByClass() const
	{
		return (ResultClass*)FindFragmentByClass(ResultClass::StaticClass());
	}

	/* Inventory Item의 인스턴스에는 무엇으로 정의되었는지 메타 클래스인 LSInventoryItemDefinition을 가지고 있다. */
	UPROPERTY()
	TSubclassOf< ULSInventoryItemDefinition> ItemDef;
};
