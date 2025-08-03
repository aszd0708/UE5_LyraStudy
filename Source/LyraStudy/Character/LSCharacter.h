// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LSCharacter.generated.h"

class ULSPawnExtensionComponent;
class ULSCameraComponent;

UCLASS()
class LYRASTUDY_API ALSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ALSCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LS|Character")
	TObjectPtr<ULSPawnExtensionComponent> PawnExtComponent;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LS|Character")
	TObjectPtr<ULSCameraComponent> CameraComponent;
};
