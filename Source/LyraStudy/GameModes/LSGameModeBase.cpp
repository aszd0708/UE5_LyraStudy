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
#include "Character/LSPawnExtensionComponent.h"

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

	// ���� GameInstance�� ����, �ʱ�ȭ �۾��� ����ǹǷ�, �� �����ӿ��� Lyra�� Concept�� Experience ó���� �����Ҽ� ����.
	// - �̸� ó���ϱ� ����, �� ������ �ڿ� �̺�Ʈ�� �޾� ó���� �̾ �����Ѵ�.
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::HandleMatchAssignmentIfNotExpectingOne);
}

void ALSGameModeBase::InitGameState()
{
	Super::InitGameState();
	
	// Experience �񵿱� �ε��� ���� Delegate�� �غ��Ѵ�
	ULSExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<ULSExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	// OnExperienceLoaded ���
	ExperienceManagerComponent->CallOrRegister_OnExperiencedLoaded(FOnLSExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

UClass* ALSGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	// GetPawnDataForController�� Ȱ���Ͽ�, PawnData�� ���� PawnClass�� ��������
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
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;
	SpawnInfo.bDeferConstruction = true;

	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		if (APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo))
		{
			// FindPawnExtensionComponent ����
			if (ULSPawnExtensionComponent* PawnExtComp = ULSPawnExtensionComponent::FindPawnExtensionComponent(SpawnedPawn))
			{
				if (const ULSPawnData* PawnData = GetPawnDataForController(NewPlayer)) 
				{
					PawnExtComp->SetPawnData(PawnData);
				}
			}
			SpawnedPawn->FinishSpawning(SpawnTransform);
			return SpawnedPawn;
		}
	}

	return nullptr;
}

void ALSGameModeBase::HandleMatchAssignmentIfNotExpectingOne()
{
	// �ش� �Լ����� �츮�� �ε��� Experience�� ���� PrimaryAssetId�� �����Ͽ�,  OnMatchAssignmentGiven���� �Ѱ��ش�.

	FPrimaryAssetId ExperienceId;

	// precedence order (highest wins)
	// - matchmaking assignment (if parent)
	// - default experience

	UWorld* World = GetWorld();

	// fall back to the default experience
	// �ϴ� �⺻ �ɼ����� default�ϰ� B_LSDefaultExperience�� �����س��´�.
	if (!ExperienceId.IsValid())
	{
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType("LSExperienceDefinition"), FName("B_LSDefaultExperience"));
	}

	// HandleMatchAssignmentIfNotExpectingOne�� OnMatchAssignmentGiven()�� ���� ���������� �ʹ��� ����
	OnMatchAssignmentGiven(ExperienceId);
}

void ALSGameModeBase::OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId)
{
	// �ش� �Լ��� ExperienceManagerComponent�� Ȱ���Ͽ� Experience�� �ε��ϱ� ����, ExperienceManagerComponent�� ServerSetCurrentExperience�� ȣ���Ѵ�.

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
	// PlayerController�� ��ȸ�ϸ�
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Cast<APlayerController>(*Iterator);

		// PlayerController�� Pawn�� Possess���� �ʾҴٸ�, RestartPlayer�� ���� Pawn�� �ٽ� Spawn �Ѵ�.
		// - OnPossess�� ����
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
	// ���� ���߿� PawnData�� �������̵� �Ǿ��� ���, PawnData�� PlayerState���� �������� ��
	if (InController)
	{
		if (const ALSPlayerState* LSPS = InController->GetPlayerState<ALSPlayerState>())
		{
			// GetPawnData ����
			if (const ULSPawnData* PawnData = LSPS->GetPawnData<ULSPawnData>())
			{
				return PawnData;
			}
		}
	}

	// fall back to the default for the current experience
	// ���� PlayerState�� PawnData�� �����Ǿ� ���� ���� ���, ExperienceManagerComponent�� CurrentExperience�� ���� �����ͼ� ����
	check(GameState);
	ULSExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<ULSExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	if (ExperienceManagerComponent->IsExperienceLoaded())
	{
		// GetExperienceChecked ����
		const ULSExperienceDefinition* Experience = ExperienceManagerComponent->GetCurrentExperienceChecked();
		if (Experience->DefaultPawnData)
		{
			return Experience->DefaultPawnData;
		}
	}

	// ��� ���̽����� �ڵ鸵�� �ȵǾ����� nullptr
	return nullptr;
}
