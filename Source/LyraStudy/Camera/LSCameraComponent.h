// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LSCameraMode.h"
#include "Camera/CameraComponent.h"
#include "LSCameraComponent.generated.h"

class ULSCameraModeStack;

/* template forward declaration */
template <class TClass> class TSubclassOf;

/* (return type, delegate_name) */
DECLARE_DELEGATE_RetVal(TSubclassOf<ULSCameraMode>, FLSCameraModeDelegate);

UCLASS()
class LYRASTUDY_API ULSCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

public:
	ULSCameraComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	static ULSCameraComponent* FindCameraComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<ULSCameraComponent>() : nullptr); }

	/*
	* member methods
	*/
	void UpdateCameraModes();

	/*
	* CameraComponent interface
	*/
	virtual void OnRegister() override;
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) final;

	/*
	* member variables
	*/
	/* 카메라의 blending 기능을 지원하는 stack */
	UPROPERTY()
	TObjectPtr<ULSCameraModeStack> CameraModeStack;

	/* 현재 CameraMode를 갖고오는 Delegate */
	FLSCameraModeDelegate DeterminCameraModeDelegate;
};
