// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/LSCameraComponent.h"
#include "Camera/LSCameraMode.h"

ULSCameraComponent::ULSCameraComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), CameraModeStack(nullptr)
{
	
}

void ULSCameraComponent::UpdateCameraModes()
{
	check(CameraModeStack);

	// DetermineCameraModeDelegate�� CameraMode Class �� ��ȯ�Ѵ�.
	// - �ش� delegate�� HeroComponent�� ��� �Լ��� ���ε� �Ǿ��ִ�.
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
		// �ʱ�ȭ (BeginPlay�� ����)�� ���� �ʿ���� ��ü�� NewObject �Ҵ�
		CameraModeStack = NewObject<ULSCameraModeStack>(this);
	}
}

void ULSCameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)
{
	Super::GetCameraView(DeltaTime, DesiredView);

	UpdateCameraModes();
}
