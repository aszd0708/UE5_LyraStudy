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
	�ش� property�� �ܼ��� ��ŷ �� �������� ���ܿ��
	- ���� ��忡 ���� GameFeature Plugin�� �ε��ϴ� �� �̿� ���� ������� �����ϸ� �ȴ�.
	- ���� ShooterCore ���� Playin �۾��� �� ���, �����ϰ� �� �����̴�
	*/
	UPROPERTY(EditDefaultsOnly, Category = GamePlay)
	TArray<FString> GameFeaturesToEnable;
};
