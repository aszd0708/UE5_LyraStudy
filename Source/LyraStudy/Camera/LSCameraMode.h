// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LSCameraMode.generated.h"

/*
* FLSCameraModeView
*/
struct FLSCameraModeView
{
	FLSCameraModeView();

	void Blend(const FLSCameraModeView& Other, float OtherWeight);
	
	FVector Location;
	FRotator Rotation;
	FRotator ControlRotation;
	float FieldOfView;
};

/*
* [0,1]�� BlendFunction�� �°� ������� ���� Ÿ��
*/
UENUM(BlueprintType)
enum class ELSCameraModeBlendFunction : uint8
{
	Linear,
	/*
	* EaseIn/Out�� exponent���� ���� ������
	*/
	EaseIn,
	EaseOut,
	EaseInOut,
	COUNT
};

class ULSCameraComponent;

/** Camera Blending ��� ���� */
UCLASS(Abstract, NotBlueprintable)
class ULSCameraMode : public UObject
{
	GENERATED_BODY()
	
public:
	ULSCameraMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/*
	* member methods
	*/
	void UpdateCameraMode(float DeltaTime);
	virtual void UpdateView(float DeltaTime);
	void UpdateBlending(float DeltaTime);

	ULSCameraComponent* GetLSCameraComponent() const;
	AActor* GetTargetActor();
	FVector GetPivotLocation();
	FRotator GetPivotRotation();

	/*
	* member variables
	*/
	/* CameraMode�� ���� ������ CameraView */
	FLSCameraModeView View;

	/* Camera Mode�� FOV */
	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "5.0", UIMax = "170", ClampMin = "5.0", Clampmax = "170.0"))
	float FieldOfView;

	/* View�� ���� Pitch [Min, Max] */
	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", Clampmax = "89.9"))
	float ViewPitchMin;

	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", Clampmax = "89.9"))
	float ViewPitchMax;

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

	/**
	* EaseIn/Out�� ����� Exponent
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendExponent;

	/** Blend function */
	ELSCameraModeBlendFunction BlendFunction;
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
	void EvaluateStack(float DeltaTime, FLSCameraModeView& OutCameraModeView);
	void UpdateStack(float DeltaTime);
	void BlendStack(FLSCameraModeView& OutCameraModeView) const;

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
