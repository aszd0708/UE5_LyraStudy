// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LSPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LYRASTUDY_API ALSPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ALSPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
