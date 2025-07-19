// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "LSPlayerState.generated.h"

class ULSPawnData;
class ULSExperienceDefinition;
/**
 * 
 */
UCLASS()
class LYRASTUDY_API ALSPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	
	/*
	* AActors' interface
	*/
	virtual void PostInitializeComponents() final;

	/*
	* member methods
	*/
	template <class T>
	const T* GetPawnData() const {return Cast<T>(PawnData); }
	void OnExperienceLoaded(const ULSExperienceDefinition* CurrentExperience);
	void SetPawnData(const ULSPawnData* InPawnData);

	UPROPERTY()
	TObjectPtr<const ULSPawnData> PawnData;
};
