// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "LSPlayerCameraManager.generated.h"

/*
* Controller 에 바인딩된 CameraManager
*/
#define LS_CAMERA_DEFAULT_FOV (80.0f)
#define LS_CAMERA_DEFAULT_PITCH_MIN (-89.0f)
#define LS_CAMERA_DEFAULT_PITCH_MAX (89.0f)

/**
 * 
 */
UCLASS()
class LYRASTUDY_API ALSPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
	
public:
	ALSPlayerCameraManager(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
