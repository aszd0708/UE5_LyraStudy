// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LyraStudy/Character/LSPawnData.h"
#include "LSExperienceDefinition.generated.h"

/**
 * 
 */
UCLASS()
class LYRASTUDY_API ULSExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	ULSExperienceDefinition(const FObjectInitializer& ObjectInitializer);

	/*
	 member variables
	*/
	UPROPERTY(EditDefaultsOnly, Category = GamePlay)
	TObjectPtr<ULSPawnData> DefaultPawnData;

	/*
	해당 property는 단순히 마킹 및 기억용으로 남겨운다
	- 게임 모드에 따른 GameFeature Plugin을 로딩하는 데 이에 대한 연결고리로 생각하면 된다.
	- 후일 ShooterCore 관련 Playin 작업을 할 경우, 진행하게 될 예정이다
	*/
	UPROPERTY(EditDefaultsOnly, Category = GamePlay)
	TArray<FString> GameFeaturesToEnable;
};
