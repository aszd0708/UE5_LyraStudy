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

	// ���� GameInstance�� ����, �ʱ�ȭ �۾��� ����ǹǷ�, �� �����ӿ��� Lyra�� Concept�� Experience ó���� �����Ҽ� ����.
	// - �̸� ó���ϱ� ����, �� ������ �ڿ� �̺�Ʈ�� �޾� ó���� �̾ �����Ѵ�.
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::HandleMatchAssignmentIfNotExpectingOne);
}

void ALSGameModeBase::HandleMatchAssignmentIfNotExpectingOne()
{
	
}
