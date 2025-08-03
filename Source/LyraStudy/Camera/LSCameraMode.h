// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LSCameraMode.generated.h"

/** Camera Blending ��� ���� */
UCLASS(Abstract, NotBlueprintable)
class ULSCameraMode : public UObject
{
	GENERATED_BODY()
	
public:
	ULSCameraMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/* �󸶵��� Blend�� �������� �ð��� �ǹ� */
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendTime;

	/* �������� Blend �� [0,1] */
	float BlendAlpha;

	/*
	* �ش� CameraMode�� ���� Blend��
	* �ռ� BlendAlpha�� ���� ���� �����Ͽ� ���� BlendWeight�� ���
	*/
	float BlendWeight;
};

/* Camera Blending�� ����ϴ� ��ü */
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

	/* ������ CameraMode�� ���� */
	UPROPERTY()
	TArray<TObjectPtr<ULSCameraMode>> CameraModeInstances;

	/* Camera Matrix Blend ������Ʈ ���� ť*/
	UPROPERTY()
	TArray<TObjectPtr<ULSCameraMode>> CameraModeStack;
};
