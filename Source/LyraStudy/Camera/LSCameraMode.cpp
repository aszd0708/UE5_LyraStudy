// Fill out your copyright notice in the Description page of Project Settings.


#include "LSCameraMode.h"

ULSCameraMode::ULSCameraMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

ULSCameraModeStack::ULSCameraModeStack(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

ULSCameraMode* ULSCameraModeStack::GetCameraModeInstance(TSubclassOf<ULSCameraMode>& CameraModeClass)
{
	check(CameraModeClass);

	// CameraModeInstacne에서 먼저 생성되어있는지 확인 후, 반환한다.
	for (ULSCameraMode* CameraMode : CameraModeInstances)
	{
		// CameraMode는 UClass를 반환한다.
		// - 즉, CameraMode는 클래스 타입에 하나만 생기게 된다.
		if ((CameraMode != nullptr) && (CameraMode->GetClass() == CameraModeClass))
		{
			return CameraMode;
		}
	}

	// CameraModeClass에 알맞는 CameraMode의 인스턴스가 없다면 생성한다.
	ULSCameraMode* NewCameraMode = NewObject<ULSCameraMode>(GetOuter(), CameraModeClass, NAME_None, RF_NoFlags);
	check(NewCameraMode);

	// 여기서 우리는 CameraModeInstances는 CameraModeClass에 맞는 인스턴스를 가지고(관리하는) 컨테이너 라고 볼수 있다.
	CameraModeInstances.Add(NewCameraMode);

	return NewCameraMode;
}

void ULSCameraModeStack::PushCameraMode(TSubclassOf<ULSCameraMode>& CameraModeClass)
{
	if (!CameraModeClass)
	{
		return;
	}

	ULSCameraMode* CameraMode = GetCameraModeInstance(CameraModeClass);
	check(CameraMode);

	int32 StackSize = CameraModeStack.Num();
	if ((StackSize > 0) && (CameraModeStack[0] == CameraMode))
	{
		// CameraModeStack[0] 가장 최근에 이미 CameraMode가 Stacking되었으므로 그냥 리턴!
		return;
	}

	// ExistingStackIndex는 CameraModeStack에서 CameraMode에 맞는 Index를 찾음
	// ExistingStackContribution은 위에서 아래로 최종 BlendWeight 값을 찾기 위해 초기값으로 1.0으로 설정
	int32 ExistingStackIndex = INDEX_NONE;
	float ExistingStackContribution = 1.0f;

	/**
	 * BlendWeight    |    ExistingStackContribution    |    ExistingStackContribution (accumulated)
	 * 0.1f           |    (1.0f) * 0.1f = 0.1f         |    (1.0f - 0.1f) = 0.9f
	 * 0.3f           |    (0.9f) * 0.3f = 0.27f        |    (1.0f - 0.3f) * 0.9f = 0.63f
	 * 0.6f           |    (0.63f) * 0.6f = 0.378f      |    (1.0f - 0.6f) * 0.63f = 0.252f
	 * 1.0f           |    (0.252f) * 1.0f = 0.252f     |
	 *                |    0.1f + 0.27f + 0.378f + 0.252f = 1.0f!
	 */
	for (int32 StackIndex = 0; StackIndex < StackSize; ++StackIndex)
	{
		if (CameraModeStack[StackIndex] == CameraMode)
		{
			ExistingStackIndex = StackIndex;
			// BlendWeight를 CameraMode에 추가해주자:
			// - 여기서 ExistingStackContribution 우리가 찾는 CameraMode를 찾았으니깐, 누적된 BlendWeight와 함께 BlendWeight을 곱하여, 루프를 빠져나온다
			ExistingStackContribution *= CameraMode->BlendWeight;
			break;
		}
		else
		{
			// 당연히 우리가 원하는 CamearMode가 아니니깐, InvBlendWeight = (1.0 - BlendWeight)를 곱해줘야, 값이 누적되겠징?
			ExistingStackContribution *= (1.0f - CameraModeStack[StackIndex]->BlendWeight);
		}
	}
	// 해당 루프의 동작 원리는 앞서 적어놓은 표를 확인해보며 이해해보자.

	// 우리는 CameraMode를 Top으로 반영하기 위해, 당연히 중간에 있었다면, 제거하여 다시 Push 해줘야 한다
	if (ExistingStackIndex != INDEX_NONE)
	{
		CameraModeStack.RemoveAt(ExistingStackIndex);
		StackSize--;
	}
	else
	{
		// 없었으니깐 당연히 Contribution은 0으로 세팅해줘야겠징?
		ExistingStackContribution = 0.0f;
	}

	// BlendTime이 0보다 크다는 것은 Blend을 얼마 시간동안 진행함을 의미 따라서, ExistingStackContribution을 적용
	// - 따라서 Blend하지 않는다면, BlendWeight를 1.0을 넣어 새로 넣는 CameraMode만 적용할 것이다
	const bool bShouldBlend = ((CameraMode->BlendTime > 0.f) && (StackSize > 0));
	const float BlendWeight = (bShouldBlend ? ExistingStackContribution : 1.0f);
	CameraMode->BlendWeight = BlendWeight;

	// 흠.. Array를 Stack처럼 사용하는것은 알지만, Index 0에 넣는건 비효율적인데..
	// - Push, Pop 메서드와 같이그냥 last에 넣는게... 어떨까 싶음
	CameraModeStack.Insert(CameraMode, 0);

	// 앞서 설명했듯이 마지막은 항상 1.0이 되어야 함!
	CameraModeStack.Last()->BlendWeight = 1.0f;
}
