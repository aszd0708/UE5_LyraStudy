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
		// ALSGameModeBase���� GetPawnDataForController�� �����ؾ� ��
		// - GetPawnDataForController���� �츮�� ���� PawnData�� �������� �ʾ����Ƿ�, ExperienceManagerComponent�� DefaultPawnData�� ����
		const ULSPawnData* NewPawnData = GameMode->GetPawnDataForController(GetOwningController());
		check(NewPawnData);

		SetPawnData(NewPawnData);
	}
}

void ALSPlayerState::SetPawnData(const ULSPawnData* InPawnData)
{
	check(InPawnData);

	// PawnData�� �ι� �����Ǵ� ���� ���� ����
	check(!PawnData);

	PawnData = InPawnData;
}
