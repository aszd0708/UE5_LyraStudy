// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/LSEquipmentDefinition.h"
#include "Equipment/LSEquipmentInstance.h"

ULSEquipmentDefinition::ULSEquipmentDefinition(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// 기본값으로 LSEqupmentInstance로 설정 (없을 경우를 대비)
	InstanceType = ULSEquipmentInstance::StaticClass();
}
