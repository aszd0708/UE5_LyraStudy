// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LSCameraMode.generated.h"

/** Camera Blending 대상 유닛 */
UCLASS(Abstract, NotBlueprintable)
class ULSCameraMode : public UObject
{
	GENERATED_BODY()
	
public:
	ULSCameraMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/* 얼마동안 Blend를 진행할지 시간을 의미 */
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendTime;

	/* 선형적인 Blend 값 [0,1] */
	float BlendAlpha;

	/*
	* 해당 CameraMode의 최종 Blend값
	* 앞서 BlendAlpha의 선형 값을 매핑하여 최종 BlendWeight를 계산
	*/
	float BlendWeight;
};

/* Camera Blending을 담당하는 객체 */
UCLASS()
class ULSCameraModeStack : public UObject
{
	GENERATED_BODY()
public:
	ULSCameraModeStack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/*
	* member variables
	*/
	ULSCameraMode* GetCameraModeInstance(TSubclassOf<ULSCameraMode>& CameraModeClass);
	void PushCameraMode(TSubclassOf<ULSCameraMode>& CameraModeClass);

	/*
	* member variables
	*/

	/* 생성된 CameraMode를 관리 */
	UPROPERTY()
	TArray<TObjectPtr<ULSCameraMode>> CameraModeInstances;

	/* Camera Matrix Blend 업데이트 진행 큐*/
	UPROPERTY()
	TArray<TObjectPtr<ULSCameraMode>> CameraModeStack;
};
