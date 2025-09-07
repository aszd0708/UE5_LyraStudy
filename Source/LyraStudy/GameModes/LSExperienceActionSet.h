// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LSExperienceActionSet.generated.h"

class UGameFeatureAction;

/**
 * 
 */
UCLASS()
class LYRASTUDY_API ULSExperienceActionSet : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	ULSExperienceActionSet();
	
	/*
	* member variables
	*/
	UPROPERTY(EditAnywhere, Category = "Actions to Perform")
	TArray<TObjectPtr<UGameFeatureAction>> Actions;
};
