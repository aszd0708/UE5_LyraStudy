// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LSGameModeBase.generated.h"

struct FPrimaryAssetId;
class ULSExperienceDefinition;
class ULSPawnData;
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
	virtual void InitGameState();

	/* GetDefaultPawnClassForController */
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

	/* HandleStartingNewPlayer */
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) final;

	/* SpawnDefaultPawnAtTransform */
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) final;

	/*
	member methods
	*/

	/// <summary>
	/// 현재 Experience가 맞는지 검증하는 함수
	/// </summary>
	void HandleMatchAssignmentIfNotExpectingOne();
	void OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId);
	bool IsExperienceLoaded() const;
	void OnExperienceLoaded(const ULSExperienceDefinition* CurrentExperience);
	const ULSPawnData* GetPawnDataForController(const AController* InController);
};
