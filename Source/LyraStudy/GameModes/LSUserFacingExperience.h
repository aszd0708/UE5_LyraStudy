// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LSUserFacingExperience.generated.h"

class UCommonSession_HostSessionRequest;

/**
 * 
 */
UCLASS()
class LYRASTUDY_API ULSUserFacingExperience : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	/*
	* Map로딩 및 Experience 전환을 위해, MapID와 ExperienceID를 활용하여, HostSexssionRequest 생성
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	UCommonSession_HostSessionRequest* CreateHostRequest() const;

	/* the specific map to load */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience, meta = (AllowedTypes = "Map"))
	FPrimaryAssetId MapID;

	/* the GamePlay experience to load */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience, meta = (AllowedTypes = "LSExperienceDefinition"))
	FPrimaryAssetId ExperienceID;
};
