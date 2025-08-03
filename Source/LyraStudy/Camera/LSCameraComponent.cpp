// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/LSCameraComponent.h"
#include "Camera/LSCameraMode.h"

ULSCameraComponent::ULSCameraComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), CameraModeStack(nullptr)
{
	
}

void ULSCameraComponent::UpdateCameraModes()
{
	check(CameraModeStack);

	// DetermineCameraModeDelegate는 CameraMode Class 를 반환한다.
	// - 해당 delegate는 HeroComponent의 멤버 함수로 바인딩 되어있다.
	if (DeterminCameraModeDelegate.IsBound())
	{
		if (const TSubclassOf<ULSCameraMode> CameraMode = DeterminCameraModeDelegate.Execute())
		{
			CameraModeStack->PushCameraMode(CameraMode);
		}
	}
}

void ULSCameraComponent::OnRegister()
{
	Super::OnRegister();

	if (!CameraModeStack)
	{
		// 초기화 (BeginPlay와 같은)가 딱히 필요없는 객체로 NewObject 할당
		CameraModeStack = NewObject<ULSCameraModeStack>(this);
	}
}

void ULSCameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)
{
	Super::GetCameraView(DeltaTime, DesiredView);

	UpdateCameraModes();
}
