// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "LSEquipementManagerComponent.generated.h"

class ULSEquipmentDefinition;
class ULSEquipmentInstance;

USTRUCT(BlueprintType)
struct FLSAppliedEquipmentEntry
{
	GENERATED_BODY()

	/* 창작물에 대한 메타 데이터 */
	UPROPERTY()
	TSubclassOf<ULSEquipmentDefinition> EquipmentDefinition;

	/* EquiipmentDefinition을 통해 생성되는 인스턴스*/
	UPROPERTY()
	TObjectPtr<ULSEquipmentInstance> Instance = nullptr;
};


/*
* EquipmentInstance의 인스턴스를 Entry에서 관리하고 있다.
* - LSEquipmentList는 생성된 객체를 관리한다고 보면 된다.
*/
USTRUCT(BlueprintType)
struct FLSEquipmentList
{
	GENERATED_BODY()

	FLSEquipmentList(UActorComponent* InOwnerComponent = nullptr) : OwnerComponent(InOwnerComponent)
	{

	}

	ULSEquipmentInstance* AddEntry(TSubclassOf<ULSEquipmentDefinition> EquipmentDefinition);
	void RemoveEntry(ULSEquipmentInstance* Instance);

	/* 창작물에 대한 관리 리스트 */
	UPROPERTY()
	TArray<FLSAppliedEquipmentEntry> Entries;

	UPROPERTY()
	TObjectPtr<UActorComponent> OwnerComponent;
};


/**
 * 
 */
UCLASS()
class LYRASTUDY_API ULSEquipementManagerComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	ULSEquipementManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	ULSEquipmentInstance* EquipItem(TSubclassOf<ULSEquipmentDefinition> EquipmentDefinition);
	void UnequipItem(ULSEquipmentInstance* ItemInstance);

	UPROPERTY()
	FLSEquipmentList EquipmentList;
};
