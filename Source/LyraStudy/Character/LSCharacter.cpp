// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LSCharacter.h"
#include "LSPawnExtensionComponent.h"
#include "Camera/LSCameraComponent.h"

ALSCharacter::ALSCharacter()
{
	// Tick�� ��Ȱ��ȭ
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bCanEverTick = false;

	// PawnExtentionComponent ����
	PawnExtComponent = CreateDefaultSubobject<ULSPawnExtensionComponent>(TEXT("PawnExtensionComponent"));

	// CameraComponent ����
	{
		CameraComponent = CreateDefaultSubobject<ULSCameraComponent>(TEXT("CameraComponent"));
		CameraComponent->SetRelativeLocation(FVector(-300.0f, 0.0f, 75.0f));
	}
}

// Called when the game starts or when spawned
void ALSCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ALSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Pawn�� Possess�μ�, Controller�� PlayerState ���� ������ ������ ���ְ� �Ǿ���:
	// - SetupPlayerInputComponent Ȯ��
	PawnExtComponent->SetupPlayerInputComponent();
}

