// Fill out your copyright notice in the Description page of Project Settings.


#include "LSGameModeBase.h"

#include "LSGameState.h"
#include "Player/LSPlayerController.h"
#include "Player/LSPlayerState.h"
#include "Character/LSCharacter.h"

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

void ALSGameModeBase::HandleMatchAssignmentIfNotExpectingOne()
{
	
}
