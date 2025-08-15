// Fill out your copyright notice in the Description page of Project Settings.


#include "LSCameraMode.h"

#include "LSPlayerCameraManager.h"
#include "LSCameraComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LSCameraMode)


FLSCameraModeView::FLSCameraModeView() : Location(ForceInit), Rotation(ForceInit), ControlRotation(ForceInit), FieldOfView(LS_CAMERA_DEFAULT_FOV)
{
}


ULSCameraMode::ULSCameraMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	FieldOfView = LS_CAMERA_DEFAULT_FOV;
	ViewPitchMin = LS_CAMERA_DEFAULT_PITCH_MIN;
	ViewPitchMax = LS_CAMERA_DEFAULT_PITCH_MAX;

	BlendTime = 0.0f;
	BlendAlpha = 1.0f;
	BlendWeight = 1.0f;

	BlendFunction = ELSCameraModeBlendFunction::EaseOut;
	BlendExponent = 4.0f;
}

void ULSCameraMode::UpdateCameraMode(float DeltaTime)
{
	// Actor�� Ȱ���Ͽ�, Pivot[Location|Rotation]�� ����Ͽ�, View�� ������Ʈ
	UpdateView(DeltaTime);

	// BlendWeight�� DeltaTime�� Ȱ���Ͽ�, BlendAlpha��� ��, BlendFunction�� �°� ������Ͽ� ���� ���
	UpdateBlending(DeltaTime);
}

void ULSCameraMode::UpdateView(float DeltaTime)
{
	// CameraMode�� ������ �ִ� CameraComponent�� Owner�� Character(Pawn0�� Ȱ���Ͽ�, PivotLocation/Rotation�� ��ȯ��
	FVector PivotLocation = GetPivotLocation();
	FRotator PivotRotation = GetPivotRotation();

	// Pitch ���� ���� Min/Max�� Clamp��Ŵ
	PivotRotation.Pitch = FMath::ClampAngle(PivotRotation.Pitch, ViewPitchMin, ViewPitchMax);

	// FLSCameraModeView�� PivotLocation/PivotRotation ����
	View.Location = PivotLocation;
	View.Rotation = PivotRotation;

	// PivoRotation�� �Ȱ��� ControlRotation���� Ȱ��
	View.ControlRotation = View.Rotation;
	View.FieldOfView = FieldOfView;

	// �����ϸ�, Character�� Location�� ControlRotation�� Ȱ���Ͽ�, View�� ������Ʈ��.
}

void ULSCameraMode::UpdateBlending(float DeltaTime)
{
	// BlendAlpha�� DeltaTime�� ���� ���
	if (BlendTime > 0.f)
	{
		// BlendTime�� Blending ���� �� �ð�(��)
		// - BlendAlpha�� 0 -> 1�� ��ȭ�ϴ� ����:
		// - DeltaTime�� Ȱ���Ͽ�, BlendTime�� 1�� �� ���, ���� ������ ����
		BlendAlpha += (DeltaTime / BlendTime);
	}
	else
	{
		BlendAlpha = 1.0f;
	}

	// BlendWeight�� [0,1]�� BlendFunction�� �°� �����
	const float Exponent = (BlendExponent > 0.0f) ? BlendExponent : 1.0f;
	switch (BlendFunction)
	{
	case ELSCameraModeBlendFunction::Linear:
		BlendWeight = BlendAlpha;
		break;
	case ELSCameraModeBlendFunction::EaseIn:
		BlendWeight = FMath::InterpEaseIn(0.0f, 1.0f, BlendAlpha, Exponent);
		break;
	case ELSCameraModeBlendFunction::EaseOut:
		BlendWeight = FMath::InterpEaseOut(0.0f, 1.0f, BlendAlpha, Exponent);
		break;
	case ELSCameraModeBlendFunction::EaseInOut:
		BlendWeight = FMath::InterpEaseInOut(0.0f, 1.0f, BlendAlpha, Exponent);
		break;
	default:
		checkf(false, TEXT("UpdateBlending: Invalid BlendFunction [%d]\n"), (uint8)BlendFunction);
		break;
	}
}

ULSCameraComponent* ULSCameraMode::GetLSCameraComponent() const
{
	// �츮�� �ռ� ULSCameraMode�� �����ϴ� ���� ULSCameraModeStack�̾���:
	// - �ش� �ڵ带 ����, GetOuter()�� ULSCameraMode�� LSCameraComponent�� �����Ͽ���
	// - ULSCameraModeStack::GetCameraModeInstance() Ȯ��
	return CastChecked<ULSCameraComponent>(GetOuter());
}

AActor* ULSCameraMode::GetTargetActor()
{
	const ULSCameraComponent* LSCameraComponent = GetLSCameraComponent();
	return LSCameraComponent->GetTargetActor();
}

FVector ULSCameraMode::GetPivotLocation()
{
	const AActor* TargetActor = GetTargetActor();
	check(TargetActor);

	if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
	{
		// BaseEyeHeight�� ����Ͽ�, ViewLocation�� ��ȯ��
		return TargetPawn->GetPawnViewLocation();
	}

	return TargetActor->GetActorLocation();
}

FRotator ULSCameraMode::GetPivotRotation()
{
	const AActor* TargetActor = GetTargetActor();
	check(TargetActor);

	if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
	{
		// GetViewRoation() Ȯ��, ���� Pawn�� ControlRotation�� ��ȯ
		return TargetPawn->GetViewRotation();
	}

	return TargetActor->GetActorRotation();
}

ULSCameraModeStack::ULSCameraModeStack(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

ULSCameraMode* ULSCameraModeStack::GetCameraModeInstance(TSubclassOf<ULSCameraMode>& CameraModeClass)
{
	check(CameraModeClass);

	// CameraModeInstacne���� ���� �����Ǿ��ִ��� Ȯ�� ��, ��ȯ�Ѵ�.
	for (ULSCameraMode* CameraMode : CameraModeInstances)
	{
		// CameraMode�� UClass�� ��ȯ�Ѵ�.
		// - ��, CameraMode�� Ŭ���� Ÿ�Կ� �ϳ��� ����� �ȴ�.
		if ((CameraMode != nullptr) && (CameraMode->GetClass() == CameraModeClass))
		{
			return CameraMode;
		}
	}

	// CameraModeClass�� �˸´� CameraMode�� �ν��Ͻ��� ���ٸ� �����Ѵ�.
	ULSCameraMode* NewCameraMode = NewObject<ULSCameraMode>(GetOuter(), CameraModeClass, NAME_None, RF_NoFlags);
	check(NewCameraMode);

	// ���⼭ �츮�� CameraModeInstances�� CameraModeClass�� �´� �ν��Ͻ��� ������(�����ϴ�) �����̳� ��� ���� �ִ�.
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
		// CameraModeStack[0] ���� �ֱٿ� �̹� CameraMode�� Stacking�Ǿ����Ƿ� �׳� ����!
		return;
	}

	// ExistingStackIndex�� CameraModeStack���� CameraMode�� �´� Index�� ã��
	// ExistingStackContribution�� ������ �Ʒ��� ���� BlendWeight ���� ã�� ���� �ʱⰪ���� 1.0���� ����
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
			// BlendWeight�� CameraMode�� �߰�������:
			// - ���⼭ ExistingStackContribution �츮�� ã�� CameraMode�� ã�����ϱ�, ������ BlendWeight�� �Բ� BlendWeight�� ���Ͽ�, ������ �������´�
			ExistingStackContribution *= CameraMode->BlendWeight;
			break;
		}
		else
		{
			// �翬�� �츮�� ���ϴ� CamearMode�� �ƴϴϱ�, InvBlendWeight = (1.0 - BlendWeight)�� �������, ���� �����ǰ�¡?
			ExistingStackContribution *= (1.0f - CameraModeStack[StackIndex]->BlendWeight);
		}
	}
	// �ش� ������ ���� ������ �ռ� ������� ǥ�� Ȯ���غ��� �����غ���.

	// �츮�� CameraMode�� Top���� �ݿ��ϱ� ����, �翬�� �߰��� �־��ٸ�, �����Ͽ� �ٽ� Push ����� �Ѵ�
	if (ExistingStackIndex != INDEX_NONE)
	{
		CameraModeStack.RemoveAt(ExistingStackIndex);
		StackSize--;
	}
	else
	{
		// �������ϱ� �翬�� Contribution�� 0���� ��������߰�¡?
		ExistingStackContribution = 0.0f;
	}

	// BlendTime�� 0���� ũ�ٴ� ���� Blend�� �� �ð����� �������� �ǹ� ����, ExistingStackContribution�� ����
	// - ���� Blend���� �ʴ´ٸ�, BlendWeight�� 1.0�� �־� ���� �ִ� CameraMode�� ������ ���̴�
	const bool bShouldBlend = ((CameraMode->BlendTime > 0.f) && (StackSize > 0));
	const float BlendWeight = (bShouldBlend ? ExistingStackContribution : 1.0f);
	CameraMode->BlendWeight = BlendWeight;

	// ��.. Array�� Stackó�� ����ϴ°��� ������, Index 0�� �ִ°� ��ȿ�����ε�..
	// - Push, Pop �޼���� ���̱׳� last�� �ִ°�... ��� ����
	CameraModeStack.Insert(CameraMode, 0);

	// �ռ� �����ߵ��� �������� �׻� 1.0�� �Ǿ�� ��!
	CameraModeStack.Last()->BlendWeight = 1.0f;
}

void ULSCameraModeStack::EvaluateStack(float DeltaTime, FLSCameraModeView& OutCameraModeView)
{
	// Top -> Bottom [0 -> Num]���� ���������� Stack�� �ִ� CameraMode ������Ʈ
	UpdateStack(DeltaTime);

	// Bottom -> Top���� CameraModeStack�� ���� Blend ����
	BlendStack(OutCameraModeView);
}

void ULSCameraModeStack::UpdateStack(float DeltaTime)
{
	const int32 StackSize = CameraModeStack.Num();
	if (StackSize <= 0)
	{
		return;
	}

	// CameraModeStack�� ��ȸ�ϸ�, CameraMode�� ������Ʈ ���ش�
	int32 RemoveCount = 0;
	int32 RemoveIndex = INDEX_NONE;
	for (int32 StackIndex = 0; StackIndex < StackSize; ++StackIndex)
	{
		ULSCameraMode* CameraMode = CameraModeStack[StackIndex];
		check(CameraMode);

		// UpdateCameraMode�� Ȯ���غ���
		CameraMode->UpdateCameraMode(DeltaTime);

		// ���� �ϳ��� CameraMode�� BlendWight 1.0�� �����ߴٸ�, �� ���� CameraMode�� �����Ѵ�.
		if (CameraMode->BlendWeight >= 1.0f)
		{
			RemoveIndex = (StackIndex + 1);
			RemoveCount = (StackSize - RemoveIndex);
			break;
		}
	}

	if (RemoveCount > 0)
	{
		CameraModeStack.RemoveAt(RemoveIndex, RemoveCount);
	}
}

void ULSCameraModeStack::BlendStack(FLSCameraModeView& OutCameraModeView) const
{
}
