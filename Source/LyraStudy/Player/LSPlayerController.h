// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularPlayerController.h"
#include "LSPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LYRASTUDY_API ALSPlayerController : public AModularPlayerController
{
	GENERATED_BODY()
	
public:
	ALSPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
