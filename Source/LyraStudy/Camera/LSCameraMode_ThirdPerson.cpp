// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/LSCameraMode_ThirdPerson.h"

#include "Curves/CurveVector.h"

ULSCameraMode_ThirdPerson::ULSCameraMode_ThirdPerson(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void ULSCameraMode_ThirdPerson::UpdateView(float DeltaTime)
{
	FVector PivotLocation = GetPivotLocation();
	FRotator PivotRotation = GetPivotRotation();

	PivotRotation.Pitch = FMath::ClampAngle(PivotRotation.Pitch, ViewPitchMin, ViewPitchMax);

	View.Location = PivotLocation;
	View.Rotation = PivotRotation;
	View.ControlRotation = View.Rotation;
	View.FieldOfView = FieldOfView;

	// TargetOffsetCurve �� �������̵� �Ǿ� �ִٸ�, Curve�� ���� �����ͼ� ���� ����
	// - Camera �������� Character�� ��� �κп� Target���� ���� �����ϴ� ������ �����ϸ� ��.
	if (TargetOffsetCurve)
	{
		const FVector TargetOffset = TargetOffsetCurve->GetVectorValue(PivotRotation.Pitch);
		View.Location = PivotLocation + PivotRotation.RotateVector(TargetOffset);
	}
}
