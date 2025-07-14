// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LSGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class LYRASTUDY_API ALSGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ALSGameModeBase();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	/*
	member methods
	*/

	/// <summary>
	/// 현재 Experience가 맞는지 검증하는 함수
	/// </summary>
	void HandleMatchAssignmentIfNotExpectingOne();
};
