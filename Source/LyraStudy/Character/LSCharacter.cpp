// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LSCharacter.h"
#include "LSPawnExtensionComponent.h"

ALSCharacter::ALSCharacter()
{
	// Tick을 비활성화
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bCanEverTick = false;

	// PawnExtentionComponent 생성
	PawnExtComponent = CreateDefaultSubobject<ULSPawnExtensionComponent>(TEXT("PawnExtensionComponent"));
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

	// Pawn이 Possess로서, Controller와 PlayerState 정보 접근이 가능한 샅애가 되었음:
	// - SetupPlayerInputComponent 확인
	PawnExtComponent->SetupPlayerInputComponent();
}

