// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/LSPlayerCameraManager.h"

ALSPlayerCameraManager::ALSPlayerCameraManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	DefaultFOV = LS_CAMERA_DEFAULT_FOV;
	ViewPitchMin = LS_CAMERA_DEFAULT_PITCH_MIN;
	ViewPitchMax = LS_CAMERA_DEFAULT_PITCH_MAX;
}
