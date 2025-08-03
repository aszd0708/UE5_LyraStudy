// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/LSPlayerController.h"
#include "Camera/LSPlayerCameraManager.h"

ALSPlayerController::ALSPlayerController(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	PlayerCameraManagerClass = ALSPlayerCameraManager::StaticClass();
}
