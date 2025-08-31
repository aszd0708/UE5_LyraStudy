// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LSMappableConfigPair.generated.h"

/* forward declaration */
class UPlayerMappableInputConfig;

USTRUCT()
struct FLSMappableConfigPair
{
	GENERATED_BODY()
public:
	FLSMappableConfigPair() = default;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UPlayerMappableInputConfig> Config;

	UPROPERTY(EditAnywhere)
	bool bShouldActivateAutomatically = true;
};