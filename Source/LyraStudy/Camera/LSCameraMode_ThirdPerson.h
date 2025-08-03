// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/LSCameraMode.h"
#include "LSCameraMode_ThirdPerson.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class LYRASTUDY_API ULSCameraMode_ThirdPerson : public ULSCameraMode
{
	GENERATED_BODY()

public:
	ULSCameraMode_ThirdPerson(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
