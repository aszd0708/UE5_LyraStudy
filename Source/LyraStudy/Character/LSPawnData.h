// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LSPawnData.generated.h"

class ULSCameraMode;
/**
 * 
 */
UCLASS()
class LYRASTUDY_API ULSPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	ULSPawnData(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	/* Pawn¿« Class*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LS|Pawn")
	TSubclassOf<APawn> PawnClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LS|Camera")
	TSubclassOf<ULSCameraMode> DefaultCameraMode;
};