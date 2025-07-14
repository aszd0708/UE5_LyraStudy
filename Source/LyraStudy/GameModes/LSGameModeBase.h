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
	/// ���� Experience�� �´��� �����ϴ� �Լ�
	/// </summary>
	void HandleMatchAssignmentIfNotExpectingOne();
};
