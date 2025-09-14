// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/LSTagedActor.h"

// Sets default values
ALSTagedActor::ALSTagedActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALSTagedActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALSTagedActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

