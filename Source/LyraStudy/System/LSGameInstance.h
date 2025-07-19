// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "LSGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class LYRASTUDY_API ULSGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	/*
	* UGameInstance's interface
	*/
	virtual void Init() override;
	virtual void Shutdown() override;
};
