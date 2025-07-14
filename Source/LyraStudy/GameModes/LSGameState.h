// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "LSGameState.generated.h"

class ULSExperienceManagerComponent;

/**
 * 
 */
UCLASS()
class LYRASTUDY_API ALSGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	ALSGameState();

public:
	UPROPERTY()
	TObjectPtr<ULSExperienceManagerComponent> ExperienceManagerComponent;
};
