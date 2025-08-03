// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/LSPlayerState.h"
#include "GameModes/LSExperienceManagerComponent.h"
#include "GameModes/LSGameModeBase.h"

void ALSPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AGameStateBase* GameState = GetWorld()->GetGameState();
	check(GameState);

	ULSExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<ULSExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	ExperienceManagerComponent->CallOrRegister_OnExperienceLoaded(FOnLSExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

void ALSPlayerState::OnExperienceLoaded(const ULSExperienceDefinition* CurrentExperience)
{
	if (ALSGameModeBase* GameMode = GetWorld()->GetAuthGameMode<ALSGameModeBase>())
	{
		// ALSGameModeBase에서 GetPawnDataForController를 구현해야 함
		// - GetPawnDataForController에서 우리느 아직 PawnData를 설정하지 않았으므로, ExperienceManagerComponent의 DefaultPawnData로 설정
		const ULSPawnData* NewPawnData = GameMode->GetPawnDataForController(GetOwningController());
		check(NewPawnData);

		SetPawnData(NewPawnData);
	}
}

void ALSPlayerState::SetPawnData(const ULSPawnData* InPawnData)
{
	check(InPawnData);

	// PawnData가 두번 설정되는 것을 원지 않음
	check(!PawnData);

	PawnData = InPawnData;
}
