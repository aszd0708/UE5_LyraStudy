// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/LSGameState.h"
#include "GameModes/LSExperienceManagerComponent.h"

ALSGameState::ALSGameState()
{
	ExperienceManagerComponent = CreateDefaultSubobject<ULSExperienceManagerComponent>(TEXT("ExperienceManagerComponent"));
}
