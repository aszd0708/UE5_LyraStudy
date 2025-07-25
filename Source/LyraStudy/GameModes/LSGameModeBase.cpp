// Fill out your copyright notice in the Description page of Project Settings.


#include "LSGameModeBase.h"
#include "LSExperienceManagerComponent.h"
#include "LSExperienceDefinition.h"
#include "LSGameState.h"
#include "LSLogChannels.h"
#include "Player/LSPlayerController.h"
#include "Player/LSPlayerState.h"
#include "Character/LSCharacter.h"
#include "Character/LSPawnData.h"

ALSGameModeBase::ALSGameModeBase()
{
	GameStateClass = ALSGameState::StaticClass();
	PlayerControllerClass = ALSPlayerController::StaticClass();
	PlayerStateClass = ALSPlayerState::StaticClass();
	DefaultPawnClass = ALSCharacter::StaticClass();
}

void ALSGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// 아직 GameInstance를 통해, 초기화 작업이 진행되므로, 현 프레임에는 Lyra의 Concept인 Experience 처리를 진행할수 없다.
	// - 이를 처리하기 위해, 한 프레임 뒤에 이벤트를 받아 처리를 이어서 진행한다.
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::HandleMatchAssignmentIfNotExpectingOne);
}

void ALSGameModeBase::InitGameState()
{
	Super::InitGameState();
	
	// Experience 비동기 로딩을 위한 Delegate를 준비한다
	ULSExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<ULSExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	// OnExperienceLoaded 등록
	ExperienceManagerComponent->CallOrRegister_OnExperiencedLoaded(FOnLSExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

UClass* ALSGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	// GetPawnDataForController를 활용하여, PawnData로 부터 PawnClass를 유도하자
	if (const ULSPawnData* PawnData = GetPawnDataForController(InController))
	{
		if (PawnData->PawnClass)
		{
			return PawnData->PawnClass;
		}
	}

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

PRAGMA_DISABLE_OPTIMIZATION
void ALSGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	if (IsExperienceLoaded())
	{
		Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	}
}
PRAGMA_ENABLE_OPTIMIZATION

APawn* ALSGameModeBase::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	UE_LOG(LogLS, Log, TEXT("SpawnDefaultPawnAtTransform_Implementation is called!"));
	return Super::SpawnDefaultPawnAtTransform_Implementation(NewPlayer, SpawnTransform);
}

void ALSGameModeBase::HandleMatchAssignmentIfNotExpectingOne()
{
	// 해당 함수에서 우리가 로딩할 Experience에 대해 PrimaryAssetId를 생성하여,  OnMatchAssignmentGiven으로 넘겨준다.

	FPrimaryAssetId ExperienceId;

	// precedence order (highest wins)
	// - matchmaking assignment (if parent)
	// - default experience

	UWorld* World = GetWorld();

	// fall back to the default experience
	// 일단 기본 옵션으로 default하게 B_LSDefaultExperience로 설정해놓는다.
	if (!ExperienceId.IsValid())
	{
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType("LSExperienceDefinition"), FName("B_LSDefaultExperience"));
	}

	// HandleMatchAssignmentIfNotExpectingOne과 OnMatchAssignmentGiven()은 아직 직관적으로 와닫지 않음
	OnMatchAssignmentGiven(ExperienceId);
}

void ALSGameModeBase::OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId)
{
	// 해당 함수는 ExperienceManagerComponent를 활용하여 Experience를 로딩하기 위해, ExperienceManagerComponent의 ServerSetCurrentExperience를 호출한다.

	check(ExperienceId.IsValid());

	ULSExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<ULSExperienceManagerComponent>();
	check(ExperienceManagerComponent);
	ExperienceManagerComponent->ServerSetCurrentExperience(ExperienceId);
}

bool ALSGameModeBase::IsExperienceLoaded() const
{
	check(GameState);
	ULSExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<ULSExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	return ExperienceManagerComponent->IsExperienceLoaded();
}

void ALSGameModeBase::OnExperienceLoaded(const ULSExperienceDefinition* CurrentExperience)
{
	// PlayerController를 순회하며
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Cast<APlayerController>(*Iterator);

		// PlayerController가 Pawn을 Possess하지 않았다면, RestartPlayer를 통해 Pawn을 다시 Spawn 한다.
		// - OnPossess를 보자
		if (PC && PC->GetPawn() == nullptr)
		{
			if (PlayerCanRestart(PC))
			{
				RestartPlayer(PC);
			}
		}
	}
}

const ULSPawnData* ALSGameModeBase::GetPawnDataForController(const AController* InController)
{
	// 게임 도중에 PawnData가 오버라이드 되었을 경우, PawnData는 PlayerState에서 가져오게 됨
	if (InController)
	{
		if (const ALSPlayerState* LSPS = InController->GetPlayerState<ALSPlayerState>())
		{
			// GetPawnData 구현
			if (const ULSPawnData* PawnData = LSPS->GetPawnData<ULSPawnData>())
			{
				return PawnData;
			}
		}
	}

	// fall back to the default for the current experience
	// 아직 PlayerState에 PawnData가 설정되어 있지 않은 경우, ExperienceManagerComponent의 CurrentExperience로 부터 가져와서 설정
	check(GameState);
	ULSExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<ULSExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	if (ExperienceManagerComponent->IsExperienceLoaded())
	{
		// GetExperienceChecked 구현
		const ULSExperienceDefinition* Experience = ExperienceManagerComponent->GetCurrentExperienceChecked();
		if (Experience->DefaultPawnData)
		{
			return Experience->DefaultPawnData;
		}
	}

	// 어떠한 케이스에도 핸들링이 안되었으면 nullptr
	return nullptr;
}
